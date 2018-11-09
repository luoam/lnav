/**
 * Copyright (c) 2007-2012, Timothy Stack
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * * Neither the name of Timothy Stack nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "view_curses.hh"
#include "vt52_curses.hh"

#if defined HAVE_NCURSESW_CURSES_H
#  include <ncursesw/curses.h>
#  include <ncursesw/term.h>
#elif defined HAVE_NCURSESW_H
#  include <ncursesw.h>
#  include <term.h>
#elif defined HAVE_NCURSES_CURSES_H
#  include <ncurses/curses.h>
#  include <ncurses/term.h>
#elif defined HAVE_NCURSES_H
#  include <ncurses.h>
#  include <term.h>
#elif defined HAVE_CURSES_H
#  include <curses.h>
#  include <term.h>
#else
#  error "SysV or X/Open-compatible Curses header file required"
#endif

#undef set_window

int main(int argc, char *argv[])
{
  int lpc, c, fd, retval = EXIT_SUCCESS;
  vt52_curses vt;
  
  fd = open("/tmp/lnav.err", O_WRONLY|O_CREAT|O_APPEND, 0666);
  dup2(fd, STDERR_FILENO);
  close(fd);
  fprintf(stderr, "startup\n");

  while ((c = getopt(argc, argv, "y:")) != -1) {
    switch (c) {
    case 'y':
      vt.set_y(atoi(optarg));
      break;
    }
  }
  
  for (lpc = 0; lpc < 1000; lpc++) {
    int len;
    
    assert(vt.map_input(random(), len) != NULL);
    assert(len > 0);
  }

  tgetent(NULL, "vt52");
  {
    static const char *CANNED_INPUT[] = {
      "abc",
      "\r",
      tgetstr((char *)"ce", NULL),
      "de",
      "\n",
      "1\n",
      "2\n",
      "3\n",
      "4\n",
      "5\n",
      "6\n",
      "7\n",
      "8\n",
      "9\n",
      "abc",
      "\x2",
      "\a",
      "ab\bcdef",
      0
    };

    screen_curses sc;

    noecho();
    vt.set_window(sc.get_window());
    vt.set_width(10);
    
    for (lpc = 0; CANNED_INPUT[lpc]; lpc++) {
      vt.map_output(CANNED_INPUT[lpc], strlen(CANNED_INPUT[lpc]));
      vt.do_update();
      refresh();
      getch();
    }

    getch();
  }
  
  return retval;
}
