// Ahungry Pset - A potential PHP extension (maybe...)
// Copyright (C) 2016 Matthew Carter
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY/ without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Better way to track our foo.type than comparing strings
 */
enum
  {
    SCALAR,
    LIST
  };

/**
 * Type allows us to have named keys, and values or lists
 */
typedef struct foo
{
  short type;
  char key[64];
  char val[64];
  struct foo *list[5];
  size_t lsize;
  int iter;
} foo;

void clone (foo *foo_ptr, foo *copy);
void dump (foo *foo_ptr, int nest);
void hasArray (foo *foo_ptr, int *found);
foo powerset (foo *foo_ptr, int *found);

/**
 * Copy one recursive struct into another
 */
void clone (foo *foo_ptr, foo *copy)
{
  // Copy our pass in
  strcpy (copy->key, foo_ptr->key);
  strcpy (copy->val, foo_ptr->val);
  copy->type = foo_ptr->type;
  copy->lsize = 0;
  copy->iter = foo_ptr->iter;

  // Deep copy all the elements
  for (unsigned i = 0u; i < foo_ptr->lsize;  i++)
    {
      foo nfoo;
      foo *nfp = &nfoo;
      copy->lsize++;

      // Main values are copied in, now copy each list
      clone (foo_ptr->list[i], nfp);
      copy->list[i] = malloc (sizeof (foo));
      *copy->list[i] = *nfp;
    }
}

/**
 * Recurse across the struct and format contents
 *
 * @param foo *foo_ptr The pointer to the foo structure
 * @param int nest The counter for how far to indent
 * @return void
 */
void dump (foo *foo_ptr, int nest)
{
  if (foo_ptr->type == SCALAR)
    {
      printf ("\"%s\"", foo_ptr->val);
    }

  if (foo_ptr->type == LIST)
    {
      //printf ("{");
      printf ("{\"%s\": ", foo_ptr->key);

      if (foo_ptr->lsize > 1)
        {
          printf ("[");
        }

      for (unsigned i = 0; i < foo_ptr->lsize; i++)
        {
          dump (foo_ptr->list[i], nest * 1 + 2);

          if (i + 1 < foo_ptr->lsize)
            {
              printf (", ");
            }
        }

      if (foo_ptr->lsize > 1)
        {
          printf ("]");
        }

      printf ("}");
    }
}

/**
 * Recurse into struct to see if a node has a multi-endpoint
 *
 * @param foo *foo_ptr The pointer to the foo structure
 * @param int *found If we found a match to reduce on
 * @return void
 */
void hasArray (foo *foo_ptr, int *found)
{
  int i, j;

  for (i = foo_ptr->iter, j = 0; i < (int) foo_ptr->lsize; i++, j++)
    {
      // If we have an unkeyed list item, we have a plain array found and can expand on it
      if (foo_ptr->list[i]->type == SCALAR       // Only plain scalars expand
          && strlen (foo_ptr->list[i]->key) == 0 // Only expand on plain array elements
          && foo_ptr->list[i + 1])               // Ensure we have a next element before expanding to this one
        {
          printf ("hasArray found match at value: %s with i: %d and lsize: %d\n",
                  foo_ptr->list[i]->val,
                  i,
                  foo_ptr->lsize
                  );

          *found = 1;
          return;
        }
      else if (foo_ptr->list[i]->type == LIST)
        {
          hasArray (foo_ptr->list[i], found);
        }
    }

  return;
}

/**
 * Recurse across the struct and format contents
 *
 * @param foo *foo_ptr The pointer to the foo structure
 * @param int *found If we found a match to reduce on
 * @return void
 */
foo powerset (foo *foo_ptr, int *found)
{
  int i;
  foo result;

  // Copy our pass in
  strcpy (result.key, foo_ptr->key);
  strcpy (result.val, foo_ptr->val);
  result.type = foo_ptr->type;
  result.lsize = 0;
  result.iter = foo_ptr->iter;

  int j;
  for (i = foo_ptr->iter, j = 0; i < (int) foo_ptr->lsize; i++, j++)//foo_ptr->list[i]; i++)
    {
      // Copy the list element over
      foo copy;
      foo *copyp = &copy;
      clone (foo_ptr->list[i], copyp);
      result.list[j] = malloc (sizeof (foo));
      *result.list[j] = *copyp;
      result.lsize++;

      // If we have an unkeyed list item, just grab first element and quit matching on others
      if (foo_ptr->list[i]->type == SCALAR       // Only plain scalars expand
          && strlen (foo_ptr->list[i]->key) == 0 // Only expand on plain array elements
          && foo_ptr->list[i + 1]                // Ensure we have a next element before expanding to this one
          && !*found)                            // Stop after first expansion
        {
          foo_ptr->iter++;
          result.iter++;
          *found = 1;
          return result;
        }
      else if (foo_ptr->list[i]->type == LIST)
        {
          *result.list[j] = powerset (foo_ptr->list[i], found);//->list[i]);
        }
    }

  return result;
}

void deviations (foo *foo_ptr)
{
  foo result;
  int found = 1;
  int canExpand = 0;
  int while_iter = 0;
  foo cloned;
  foo *cloned_ptr = &cloned;

  //printf ("\n\nENTER DEVIATION...\n\n");

  while (found && while_iter++ < 20)
    {
      // Avoid eating all the cpu
      usleep (1000); // 1ms

      //printf ("\n\nBegin to expand:\n");
      //dump (foo_ptr, 0);

      found = 0;
      //canExpand = 0;
      result = powerset (foo_ptr, &found);

      //printf ("\nExpanded to:\n");
      //dump (&result, 0);

      //deviations (&result);
      canExpand = 0;
      clone (&result, cloned_ptr);
      hasArray (cloned_ptr, &canExpand);

      if (canExpand == 1)
        {
          deviations (cloned_ptr);
          //printf ("We could expand our result set....\n");
        }
      else
        {
          //printf ("NO FURTHER EXPANSION:\n");
          printf ("\n");
          dump (cloned_ptr, 0);
        }

      // Copy result into foo_ptr
      //clone (&result, foo_ptr);
    }

  return;
}

/**
 * Do we need to describe this?
 *
 * @param int argc How many arguments passed in
 * @param char *argv[] The array of string arguments
 * @return int Success (0) or failure (!0)
 */
int main (int argc, char *argv[])
{
  // Set up the equivalent of the following PHP array:
  // $arr = ['a' => [1, 2], 'b' => [3, 4]]
  foo a2 = { SCALAR, "\0", "2", { NULL }, 0, 0 };
  foo a1 = { SCALAR, "\0", "1", { NULL }, 0, 0 };
  foo b2 = { SCALAR, "\0", "4", { NULL }, 0, 0 };
  foo b1 = { SCALAR, "\0", "3", { NULL }, 0, 0 };
  foo a = { LIST, "a", "", { &a1, &a2 }, 2, 0 };
  foo b = { LIST, "b", "", { &b1, &b2 }, 2, 0 };
  foo foo1 = { LIST, "result", "", { &a, &b }, 2, 0 };

  // Set up while loop variables
  foo *foo_ptr = &foo1;
  deviations (foo_ptr);

  printf ("OK\n");

  return 0;
}
