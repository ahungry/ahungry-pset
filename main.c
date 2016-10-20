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
foo powerset (foo *foo_ptr, int iter);

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
  copy->iter = 0;

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
 * Recurse across the struct and format contents
 *
 * @param foo *foo_ptr The pointer to the foo structure
 * @param int nest The counter for how far to indent
 * @return void
 */
foo powerset (foo *foo_ptr, int iter)
{
  int i;
  foo result;

  // Copy our pass in
  strcpy (result.key, foo_ptr->key);
  strcpy (result.val, foo_ptr->val);
  result.type = foo_ptr->type;
  result.lsize = 0;
  result.iter = 0;

  for (i = 0u; foo_ptr->list[i]; i++)
    {
      // Copy the list element over
      // WOOPS this is linking the elements
      foo copy;
      foo *copyp = &copy;
      clone (foo_ptr->list[i], copyp);
      result.list[i] = malloc (sizeof (foo));
      *result.list[i] = *copyp;
      //result.list[i] = foo_ptr->list[i];
      result.lsize++;

      // If we have an unkeyed list item, just grab first element and quit matching on others
      if (foo_ptr->list[i]->type == SCALAR && strlen (foo_ptr->list[i]->key) == 0)
        {
          //printf ("linking pher value (%s) here...\n", foo_ptr->list[i]->val);
          result.list[i + 1] = NULL;
          foo_ptr->iter = iter;
          return result;
        }
      else if (foo_ptr->list[i]->type == LIST)
        {
          *result.list[i] = powerset (foo_ptr->list[i], iter + 1);//->list[i]);
        }
    }

  return result;
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

  foo *foo_ptr = &foo1;
  foo result;

  printf ("\n\nStart with:\n");
  dump (foo_ptr, 0);

  foo cloned;
  foo *cloned_ptr = &cloned;
  clone (foo_ptr, cloned_ptr);

  printf ("\n\nThen we clone it:\n");
  dump (cloned_ptr, 0);

  result = powerset (foo_ptr, 0);

  printf ("\n\nGenerates result:\n");
  dump (&result, 0);

  printf ("\n\nLeaving us with:\n");
  dump (foo_ptr, 0);

  printf ("\n\nIn clone:\n");
  dump (cloned_ptr, 0);

  //printf ("Its top value is: %s", cloned_ptr->list[0]->key);

  printf ("\n\n");
  return 0;

  foo result2;
  result2 = powerset (foo_ptr, 0);
  dump (&result2, 0);


  return 0;
}
