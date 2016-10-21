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

#define MAX_WHILE_NEST 20

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
typedef struct puny_hash
{
  short type;
  char key[64];
  char val[64];
  struct puny_hash *list[5];
  size_t lsize;
  int iter;
} PunyHash;

void clone (PunyHash *puny_hash_ptr, PunyHash *copy);
void deviations (PunyHash *puny_hash_ptr, int *comma);
void dump (PunyHash *puny_hash_ptr, int nest);
void hasArray (PunyHash *puny_hash_ptr, int *found);
PunyHash powerset (PunyHash *puny_hash_ptr, int *found);

/**
 * Copy one recursive struct into another
 */
void clone (PunyHash *puny_hash_ptr, PunyHash *copy)
{
  // Copy our pass in
  strcpy (copy->key, puny_hash_ptr->key);
  strcpy (copy->val, puny_hash_ptr->val);
  copy->type = puny_hash_ptr->type;
  copy->lsize = 0;
  copy->iter = 0;//puny_hash_ptr->iter;

  // Deep copy all the elements
  for (unsigned i = 0u; i < puny_hash_ptr->lsize;  i++)
    {
      PunyHash npuny_hash;
      PunyHash *nfp = &npuny_hash;
      copy->lsize++;

      // Main values are copied in, now copy each list
      clone (puny_hash_ptr->list[i], nfp);
      copy->list[i] = malloc (sizeof (PunyHash));
      *copy->list[i] = *nfp;
    }
}

/**
 * Generate all possible deviations of the powerset
 *
 * @param puny_hash *puny_hash_ptr The puny_hash pointer reference
 * @param int *comma If we want to track a comma or not
 * @return void
 */
void deviations (PunyHash *puny_hash_ptr, int *comma)
{
  PunyHash result;
  int found = 1;
  int canExpand = 0;
  int while_iter = 0;
  PunyHash cloned;
  PunyHash *cloned_ptr = &cloned;

  //printf ("\n\nENTER DEVIATION...\n\n");

  while (found && while_iter++ < MAX_WHILE_NEST)
    {
      // Ok, some weird timing issue - without this, it infinitely
      // loops, so I'm probably missing something with memory
      // management to avoid a conflict in it.
      usleep (1); // Almost non-existent sleep interval

      found = 0;
      result = powerset (puny_hash_ptr, &found);

      canExpand = 0;
      clone (&result, cloned_ptr);
      hasArray (cloned_ptr, &canExpand);

      if (canExpand == 1)
        {
          deviations (cloned_ptr, comma);
        }
      else
        {
          if (*comma)
            {
              printf (",");
            }

          *comma = 1;
          printf ("\n");
          dump (cloned_ptr, 0);
        }
    }

  return;
}

/**
 * Recurse across the struct and format contents
 *
 * @param foo *foo_ptr The pointer to the foo structure
 * @param int nest The counter for how far to indent
 * @return void
 */
void dump (PunyHash *puny_hash_ptr, int nest)
{
  if (puny_hash_ptr->type == SCALAR)
    {
      printf ("\"%s\"", puny_hash_ptr->val);
    }

  if (puny_hash_ptr->type == LIST)
    {
      //printf ("{");
      printf ("{\"%s\": ", puny_hash_ptr->key);

      if (puny_hash_ptr->lsize > 1)
        {
          printf ("[");
        }

      for (unsigned i = 0; i < puny_hash_ptr->lsize; i++)
        {
          dump (puny_hash_ptr->list[i], nest * 1 + 2);

          if (i + 1 < puny_hash_ptr->lsize)
            {
              printf (", ");
            }
        }

      if (puny_hash_ptr->lsize > 1)
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
void hasArray (PunyHash *puny_hash_ptr, int *found)
{
  int i, j;

  for (i = puny_hash_ptr->iter, j = 0; i < (int) puny_hash_ptr->lsize; i++, j++)
    {
      // If we have an unkeyed list item, we have a plain array found and can expand on it
      if (puny_hash_ptr->list[i]->type == SCALAR       // Only plain scalars expand
          && strlen (puny_hash_ptr->list[i]->key) == 0 // Only expand on plain array elements
          && puny_hash_ptr->list[i + 1])               // Ensure we have a next element before expanding to this one
        {
          *found = 1;
          return;
        }
      else if (puny_hash_ptr->list[i]->type == LIST)
        {
          hasArray (puny_hash_ptr->list[i], found);
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
PunyHash powerset (PunyHash *puny_hash_ptr, int *found)
{
  int i;
  PunyHash result;

  // Copy our pass in
  strcpy (result.key, puny_hash_ptr->key);
  strcpy (result.val, puny_hash_ptr->val);
  result.type = puny_hash_ptr->type;
  result.lsize = 0;
  result.iter = puny_hash_ptr->iter;

  int j;
  for (i = puny_hash_ptr->iter, j = 0; i < (int) puny_hash_ptr->lsize; i++, j++)//puny_hash_ptr->list[i]; i++)
    {
      // Copy the list element over
      PunyHash copy;
      PunyHash *copyp = &copy;
      clone (puny_hash_ptr->list[i], copyp);
      result.list[j] = malloc (sizeof (PunyHash));
      *result.list[j] = *copyp;
      result.lsize++;

      // If we have an unkeyed list item, just grab first element and quit matching on others
      if (puny_hash_ptr->list[i]->type == SCALAR       // Only plain scalars expand
          && strlen (puny_hash_ptr->list[i]->key) == 0 // Only expand on plain array elements
          && puny_hash_ptr->list[i + 1]                // Ensure we have a next element before expanding to this one
          && !*found)                            // Stop after first expansion
        {
          puny_hash_ptr->iter++;
          result.iter++;
          *found = 1;
          return result;
        }
      else if (puny_hash_ptr->list[i]->type == LIST)
        {
          *result.list[j] = powerset (puny_hash_ptr->list[i], found);//->list[i]);
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
  if (argc > 1)
    {
      printf ("Invalid option: '%s'\n\n", argv[1]);
      printf ("Usage: No support for input yet, just run the command...\n");

      return 1;
    }

  // Set up the equivalent of the following PHP array:
  // $arr = ['a' => [1, 2], 'b' => [3, 4]]
  PunyHash a2 = { SCALAR, "\0", "2", { NULL }, 0, 0 };
  PunyHash a1 = { SCALAR, "\0", "1", { NULL }, 0, 0 };
  PunyHash b2 = { SCALAR, "\0", "4", { NULL }, 0, 0 };
  PunyHash b1 = { SCALAR, "\0", "3", { NULL }, 0, 0 };
  PunyHash a = { LIST, "a", "", { &a1, &a2 }, 2, 0 };
  PunyHash b = { LIST, "b", "", { &b1, &b2 }, 2, 0 };
  PunyHash puny_hash1 = { LIST, "data", "", { &a, &b }, 2, 0 };

  // Set up while loop variables
  PunyHash *puny_hash_ptr = &puny_hash1;
  int comma = 0;

  printf ("{\"original\": ");
  dump (puny_hash_ptr, 0);
  printf (",\n\"deviations\": [");
  deviations (puny_hash_ptr, &comma);
  printf ("]}\n");

  return 0;
}
