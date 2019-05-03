#include "logger.hpp"

int main()
{
   LOG("Starting the application..");
   for(int i = 0; i < 5; i++)
   {
     LOG("Loop iteration number: " + std::to_string(i));
   }
   LOG_WARN("Loop is over");

   LOG_ERR("And the program ends"); // probably this will be not saved

   return 0;
}
