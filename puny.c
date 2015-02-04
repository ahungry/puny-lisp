/*
 * Puny Lisp - The weakest lisp around!
 * Copyright (C) 2015 Matthew Carter <m@ahungry.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
//#include <curl/curl.h>
//#include <sys/types.h>
//#include <regex.h>

#define MAXBUFFERSIZE 80
#define MAXFUNCTIONS 10
#define MAXARGS 10
#define MAXARGSIZE 10

void clear_to_end (void);
void fn_error (char* message);
int fn_basic_math (char op, int a, int b);
void parse_innermost (char* line);
int parse_buffer (char* line);


void clear_to_end (void)
{
  char ch;
  ch = getchar ();
  while (ch != '\n')
    {
      ch = getchar ();
    }
}

void fn_error (char* message)
{
  printf ("%s", message);
}

int fn_basic_math (char op, int a, int b)
{
  switch (op)
    {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return a / b;
    default: return a;
    }
}

/*
 * Seek until we hit an innermost pair of parens
 */
void parse_innermost (char* line)
{
  printf ("Working on: %s\n", line);
  int pos = 0, length = 0;
  for (int i = 0; i < strlen (line); i++)
    {
      length++;
      if(line[i] == '(')
        {
          pos    = i;
          length = 0;
        }
      else if (line[i] == ')')
        {
          break;
        }
    }

  // Break off the inner portion into it's own string for now.
  char subseq[length + 2];
  memcpy (subseq, &line[pos], length + 1);
  subseq[length + 1] = 0x00;
  printf ("Inner: [%s]\n", subseq);

  parse_buffer (subseq);

  // Re-assemble the buffers that we broke apart earlier.
  int  sufc = strlen (line) - pos + length + 1;
  char pre[pos];
  char suf[sufc];

  int  mergedc = strlen (pre) + strlen (suf) + strlen (subseq);
  char merged[mergedc];

  // If this is called without a pre available it butchers the result.
  if (pos > 0)
    {
      memcpy (pre, &line[0], pos);
      pre[pos + 1] = 0x00;
      sprintf (merged, "%s", pre);
    }

  sprintf (merged, "%s%s", merged, subseq);

  if (sufc > 0)
    {
      memcpy (suf, &line[pos + length + 1], sufc);
      suf[sufc + 1] = 0x00;
      sprintf (merged, "%s%s", merged, suf);
    }

  //printf ("Pre: [%s]\n", pre);
  //printf ("Eval: [%s]\n", subseq);
  //printf ("Suf: [%s]\n", suf);
  //printf ("Merged glory: [%s]\n", merged);

  strcpy (line, merged);

  if (strstr (line, "("))
    {
      parse_innermost (line);
    }
}

int parse_buffer (char* line)
{
  //printf ("\n\nPB: %s\n\n", line);
  int poc = 0; // Keep count of parenthesis to make a balance.
  int pcc = 0; // Also keep track of closed.
  int _   = 0; // Watch for spaces to split up our input.
  int pp  = 0; // Parenthesis predicate (or other special chars)

  char fn[MAXFUNCTIONS][MAXBUFFERSIZE]; // Function name to call.
  int  fnc  = 0; // Function counter.
  int  fncc = 0; // Function character counter.

  char args[MAXFUNCTIONS][MAXARGS][MAXARGSIZE]; // The arguments to apply to function.
  int  argsc  = 0; // Args counter.
  int  argscc = 0; // Args character counter.
  int  arg_size[MAXFUNCTIONS]; // Keep track of how many arguments each function has.

  for (int i = 0; i < strlen (line); i++)
    {
      switch (line[i])
        {
        case '(': poc++; pp = 1; _ = 0; break;
        case ')': pcc++; _ = 1; break;
        case ' ': _ = 1; break;
        default: _ = 0; break;
        }

      // Find what function we are doing.
      if (_) // If our char is a space (or ending paren), end the function name (or arg)
        {
          if (pp) // If we were doing function parsing.
            {
              pp = 0; // Turn off paren predicate.
              fn[fnc][fncc] = 0x00; // Null byte terminate the function name.
              printf ("The function was: %s\n", fn[fnc]);
              fnc++; // Add to the function stack.
              fncc = 0; // The function character counter.
            }
          else // Otherwise, assume it was argument parsing.
            {
              args[fnc - 1][argsc][argscc] = 0x00; // Null byte terminate the arg value.
              arg_size[fnc - 1] = argsc;
              printf ("The argument was: %s\n", args[fnc - 1][argsc]);
              argsc++; // Add to the argument stack.
              argscc = 0; // Reset the args character counter.
            }
        }
      else if (pp) // If we are in paren predicate, add to the function name
        {
          if (line[i] != '(')
            {
              fn[fnc][fncc++] = line[i];
            }
        }
      else // Otherwise, add to the argument name
        {
          if (line[i] != ')')
            {
              args[fnc - 1][argsc][argscc++] = line[i];
            }
        }
    }

  // Parse out our functions in reverse order.
  for (int i = fnc - 1; i > -1; i--)
    {
      if (strlen (fn[i]) == 1
          && (fn[i][0] == '+'
              || fn[i][0] == '-'
              || fn[i][0] == '*'
              || fn[i][0] == '/'))
        {
          // We can math multiple numbers in a single call.
          int a, b = strtol (args[fnc - 1][0], NULL, 10);

          // Loop across arguments.
          for (int ac = 1; ac <= arg_size[fnc - 1]; ac++)
            {
              a = strtol (args[fnc - 1][ac], NULL, 10);
              b = fn_basic_math (fn[i][0], b, a);
            }

          printf ("%d\n", b);
          sprintf (line, "%d", b);
        }
      else if (strcmp ("print", fn[i]) == 0)
        {
          // Do a basic print of the arguments sent in
          for (int ac = 0; ac <= arg_size[fnc - 1]; ac++)
            {
              printf ("%s ", args[fnc - 1][ac]);
            }
          printf ("\n"); // Courtesy newline
        }
      else
        {
          fn_error ("Unknown function!\n");
        }
    }

  if (poc != pcc)
    {
      printf ("Imbalanced parenthesis! Croaaak...\n");
      return 0; // A non-fatal failure in matching parens.
    }

  return 0; // A general success in parsing.
}

/*
 * Start
 */
int main (int argc, char *argv[])
{
  char ch;
  char buffer[MAXBUFFERSIZE];
  int  char_count;
  int  exit_flag = 0;

  while (exit_flag == 0)
    {
      printf ("puny> ");
      ch = getchar ();
      char_count = 0;

      while ((ch != '\n') && (char_count < MAXBUFFERSIZE))
        {
          buffer[char_count++] = ch;
          ch = getchar ();
        }

      buffer[char_count] = 0x00; /* null terminate */

      if (strcmp (buffer, "exit") == 0 || strcmp (buffer, "q") == 0)
        {
          exit_flag = 1;
        }
      else
        {
          parse_innermost (buffer);
          //exit_flag = parse_buffer (buffer);
          //printf ("%s\n", buffer);
        }
    }

  printf ("Goodbye!");
  return 0;
}
