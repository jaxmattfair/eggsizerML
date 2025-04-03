#include "../include/eggsizerml.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "--somefakecall") == 0) {
      // run some CLI call here
    }
  } else {
    QApplication a(argc, argv);
    eggsizerML w;
    w.show();
    return a.exec();
  }
}
