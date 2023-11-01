#include <cstring>

namespace ggm {
    #define PATHSEP ':'

    // Extracted from article on string matching found @ https://www.codeproject.com/Articles/5163931/Fast-String-Matching-with-Wildcards-Globs-and-Giti#:~:text=Non%2Drecursive%20gitignore%2Dstyle%20glob%20Matching
    // Article author: Robert van Engelen
    // July 22nd 2023; Rev 31
    // Returns TRUE if text string matches gitignore-style glob pattern
    int gitignore_glob_match(const char *text, const char *glob)
    {
      const char *text1_backup = NULL;
      const char *glob1_backup = NULL;
      const char *text2_backup = NULL;
      const char *glob2_backup = NULL;
      // match pathname if glob contains a / otherwise match the basename
      if (*glob == ':')
      {
        // if pathname starts with ./ then ignore these pairs
        while (*text == '.' && text[1] == PATHSEP)
          text += 2;
        // if pathname starts with / then ignore it
        if (*text == PATHSEP)
          text++;
        glob++;
      }
      else if (strchr(glob, ':') == NULL)
      {
        const char *sep = strrchr(text, PATHSEP);
        if (sep)
          text = sep + 1;
      }
      while (*text != '\0')
      {
        switch (*glob)
        {
          case '*':
            if (*++glob == '*')
            {
              // trailing ** match everything after /
              if (*++glob == '\0')
                return true;
              // ** followed by a / match zero or more directories
              if (*glob != ':')
                return false;
              // new **-loop, discard *-loop
              text1_backup = NULL;
              glob1_backup = NULL;
              text2_backup = text;
              glob2_backup = ++glob;
              continue;
            }
            // trailing * matches everything except /
            text1_backup = text;
            glob1_backup = glob;
            continue;
          case '?':
            // match any character except /
            if (*text == PATHSEP)
              break;
            text++;
            glob++;
            continue;
          case '[':
          {
            int lastchr;
            int matched = false;
            int reverse = glob[1] == '^' || glob[1] == '!' ? true : false;
            // match any character in [...] except /
            if (*text == PATHSEP)
              break;
            // inverted character class
            if (reverse)
              glob++;
            // match character class
            for (lastchr = 256; *++glob != '\0' && *glob != ']'; lastchr = *glob)
              if (lastchr < 256 && *glob == '-' && glob[1] != ']' && glob[1] != '\0' ?
                  *text <= *++glob && *text >= lastchr :
                  *text == *glob)
                matched = true;
            if (matched == reverse)
              break;
            text++;
            if (*glob != '\0')
              glob++;
            continue;
          }
          case '\\':
            // literal match \-escaped character
            glob++;
            // FALLTHROUGH
          default:
            // match the current non-NUL character
            if (*glob != *text && !(*glob == ':' && *text == PATHSEP))
              break;
            text++;
            glob++;
            continue;
        }
        if (glob1_backup != NULL && *text1_backup != PATHSEP)
        {
          // *-loop: backtrack to the last * but do not jump over /
          text = ++text1_backup;
          glob = glob1_backup;
          continue;
        }
        if (glob2_backup != NULL)
        {
          // **-loop: backtrack to the last **
          text = ++text2_backup;
          glob = glob2_backup;
          continue;
        }
        return false;
      }
      // ignore trailing stars
      while (*glob == '*')
        glob++;
      // at end of text means success if nothing else is left to match
      return *glob == '\0' ? true : false;
    }
}
