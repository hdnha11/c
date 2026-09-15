#include <stdio.h>
#include <string.h>

int matchhere(char *regexp, char *text);
int matchstar(int c, char *regexp, char *text);

/* match: search for regexp anywhere in text */
int match(char *regexp, char *text) {
  if (regexp[0] == '^')
    return matchhere(regexp + 1, text);
  do { /* must look even if string is empty */
    if (matchhere(regexp, text))
      return 1;
  } while (*text++ != '\0');
  return 0;
}

/* matchhere: search for regexp at beginning of text */
int matchhere(char *regexp, char *text) {
  if (regexp[0] == '\0')
    return 1;
  if (regexp[1] == '*')
    return matchstar(regexp[0], regexp + 2, text);
  if (regexp[0] == '$' && regexp[1] == '\0')
    return *text == '\0';
  if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
    return matchhere(regexp + 1, text + 1);
  return 0;
}

/* matchstar: search for c*regexp at beginning of text */
int matchstar(int c, char *regexp, char *text) {
  do { /* a * matches zero or more instances */
    if (matchhere(regexp, text))
      return 1;
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}

/* Test Helper */

static int test_count = 0;

void run_test(char *regexp, char *text, int expected) {
  int result = match(regexp, text);
  test_count++;
  if (result == expected) {
    printf("[PASS] Test %2d: match(\"%s\", \"%s\") == %d\n", test_count, regexp,
           text, expected);
  } else {
    printf("[FAIL] Test %2d: match(\"%s\", \"%s\") expected %d, got %d\n",
           test_count, regexp, text, expected, result);
  }
}

void test_matcher() {
  printf("--- Running Matcher Tests ---\n\n");

  /* Literal Matches */
  printf("Literal Matches:\n");
  run_test("a", "a", 1);
  run_test("hello", "hello world", 1);
  run_test("world", "hello world", 1);
  run_test("xyz", "hello world", 0);
  run_test("", "anything", 1); /* Empty regex matches anywhere */
  run_test("abc", "", 0);

  /* Wildcard ('.' matches any single character) */
  printf("\nWildcard '.' Matches:\n");
  run_test("a.c", "abc", 1);
  run_test("a.c", "aXc", 1);
  run_test("a.c", "ac", 0); /* '.' must match at least 1 character */
  run_test(".a.", "cat", 1);

  /* Anchors ('^' start, '$' end) */
  printf("\nAnchors (^ and $):\n");
  run_test("^hello", "hello world", 1);
  run_test("^world", "hello world", 0);
  run_test("world$", "hello world", 1);
  run_test("hello$", "hello world", 0);
  run_test("^hello world$", "hello world", 1);
  run_test("^hello$", "hello world", 0);
  run_test("^$", "", 1); /* Empty string anchored start and end */
  run_test("^$", "a", 0);

  /* Star Quantifier ('*' zero or more) */
  printf("\nStar '*' Quantifier:\n");
  run_test("a*", "", 1);        /* 0 matches */
  run_test("a*", "aaaa", 1);    /* Multiple matches */
  run_test("ab*c", "ac", 1);    /* 0 'b's */
  run_test("ab*c", "abbbc", 1); /* Multiple 'b's */
  run_test("ab*c", "abxd", 0);
  run_test(".*", "anything goes", 1); /* Match everything */
  run_test(".*end$", "this is the end", 1);

  /* Edge Cases & Complex Combinations */
  printf("\nEdge Cases:\n");
  run_test("^a.*z$", "a VERY long string z", 1);
  run_test("^a.*z$", "a VERY long string y", 0);
  run_test("a*b*c*", "aaabbbccc", 1);
  run_test("a*b*c*", "", 1);

  printf("\nCompleted %d tests.\n", test_count);
}

/* --- REPL Helper Functions --- */

/* Utility to strip trailing newlines from fgets input */
static void strip_newline(char *str) {
  size_t len = strlen(str);
  if (len > 0 && str[len - 1] == '\n') {
    str[len - 1] = '\0';
  }
}

void repl() {
  char regexp[256];
  char text[256];

  printf("=========================================\n");
  printf("   Regex Matcher Interactive REPL        \n");
  printf("   Type 'quit' as regex to exit.         \n");
  printf("=========================================\n\n");

  while (1) {
    printf("regex> ");
    if (!fgets(regexp, sizeof(regexp), stdin))
      break;
    strip_newline(regexp);

    /* Exit check */
    if (strcmp(regexp, "quit") == 0) {
      printf("Exiting REPL. Goodbye!\n");
      break;
    }

    printf("text > ");
    if (!fgets(text, sizeof(text), stdin))
      break;
    strip_newline(text);

    /* Evaluate match */
    int is_match = match(regexp, text);

    if (is_match) {
      printf("\033[1;32m[MATCH]\033[0m \"%s\" matches inside \"%s\"\n\n",
             regexp, text);
    } else {
      printf(
          "\033[1;31m[NO MATCH]\033[0m \"%s\" does NOT match inside \"%s\"\n\n",
          regexp, text);
    }
  }
}

int main(void) {
  test_matcher();

  repl();

  return 0;
}
