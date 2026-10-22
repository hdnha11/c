#include <stdio.h>

int main(void) {
  for (char c = 30; c < 38; c++) {
    printf("\033[%dmColor\033[0m\t", c);
    printf("\033[%dmBright\033[0m\t", c + 60);
    printf("\033[%d;1mBold\033[0m\t", c);
    printf("\033[%d;2mLight\033[0m\t", c);
    printf("\033[%d;3mItalic\033[0m\t", c);
    printf("\033[%d;4mUnderline\033[0m\t", c);
    printf("\033[%d;5mBlinking\033[0m\t", c);
    printf("\033[%d;6mRapid\033[0m\t", c);
    printf("\033[%d;7mInverted\033[0m\t", c);
    printf("\033[%d;8mConceal\033[0m\t", c);
    printf("\033[%d;9mStrike\033[0m\t", c);
    printf("\033[%dmBackground\033[0m\t", c + 10);
    printf("\033[%dmBright background\033[0m\t", c + 10 + 60);
    printf("\n");
  }
  return 0;
}
