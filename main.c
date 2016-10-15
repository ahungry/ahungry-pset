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
} foo;

/**
 * Recurse across the struct and format contents
 *
 * @param foo* foo_ptr The pointer to the foo structure
 * @param int nest The counter for how far to indent
 * @return void
 */
void dump (foo* foo_ptr, int nest)
{
  int i;
  for (i = 0u; foo_ptr->list[i]; i++)
    {
      for (int c = 0; c < nest; c++)
        {
          printf (" ");
        }

      if (foo_ptr->list[i]->type == SCALAR)
        {
          printf ("'%s' => '%s',\n", foo_ptr->list[i]->key, foo_ptr->list[i]->val);
        }
      else
        {
          printf ("'%s' => [\n", foo_ptr->list[i]->key);
          dump (foo_ptr->list[i], nest + 2);
          printf ("],\n");
        }
    }
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
  printf ("Running %s, We saw %d args...\n", argv[0], argc);
  printf ("Scalar: %d, List: %d", SCALAR, LIST);

  // Set up the equivalent of the following PHP array:
  // $arr = ['a' => [1, 2], 'b' => [3, 4]]
  foo a2 = { SCALAR, "\0", "2", { NULL } };
  foo a1 = { SCALAR, "\0", "1", { NULL } };
  foo b2 = { SCALAR, "\0", "4", { NULL } };
  foo b1 = { SCALAR, "\0", "3", { NULL } };
  foo a = { LIST, "a", "", { &a1, &a2 } };
  foo b = { LIST, "b", "", { &b1, &b2 } };
  foo foo1 = { LIST, "\0", "", { &a, &b } };

  printf ("k: %s, v: %s\n", foo1.list[0]->key, foo1.list[0]->val);

  foo *foo_ptr = &foo1;

  printf ("Its loopin time\n");
  dump (foo_ptr, 0);

  return 0;
}
