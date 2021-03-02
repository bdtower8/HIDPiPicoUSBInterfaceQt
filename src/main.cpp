#include "application.h"

int main(int argc, char *argv[])
{
    Application *app = new Application();
    return app->run(argc, argv);
}
