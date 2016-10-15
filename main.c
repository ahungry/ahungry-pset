// http://boredzo.org/pointers/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum
  {
    SCALAR,
    LIST
  };

// Each thing inside here is a member
typedef struct foo
{
  short type;
  char key[64];
  char val[64];
  struct foo *list[5];
} foo;

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

int main (int argc, char *argv[])
{
  printf ("Running %s, We saw %d args...\n", argv[0], argc);

  printf ("Scalar: %d, List: %d", SCALAR, LIST);

  foo a2 = { SCALAR, "\0", "2", { NULL } };
  foo a1 = { SCALAR, "\0", "1", { NULL } };
  foo b2 = { SCALAR, "\0", "4", { NULL } };
  foo b1 = { SCALAR, "\0", "3", { NULL } };
  foo a = { LIST, "a", "", { &a1, &a2 } };
  foo b = { LIST, "b", "", { &b1, &b2 } };
  foo foo1 = { LIST, "\0", "", { &a, &b } };

  printf ("k: %s, v: %s\n", foo1.list[0]->key, foo1.list[0]->val);

  foo *foo_ptr = &foo1;
  //foo_ptr = foo_ptr->list[0];

  printf ("Its loopin time\n");
  dump (foo_ptr, 0);

  return 0;
}
