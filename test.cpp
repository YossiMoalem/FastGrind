#include <stdlib.h>
#include <iostream>

class A 
{
   public :
   A() 
   {
      std::cout <<"in CTor\n";
   }
   ~A()
   {
      std::cout <<"In dTor\n";
   }
   int mI;
};

int main ()
{
   /*
   std::cout <<"+ should be cuorruption at end\n";
   int *i = new int;
   int * i2 = i;
   i2++;
   *i2 = 1;
   delete i;

   std::cout <<"+ should call ctor and dtor \n";
   A* pA = new A;
   delete pA;
   std::cout <<"+ should be cuorruption at end\n";
   int * arr = new int[10];
   arr[10] = 10;
   delete arr;
   std::cout <<"+ should be cuorruption at begin\n";
   int * arr2 = new int[10];
   arr2[-1] = 10;
   delete arr2;

   int* a = (int *) malloc (sizeof(int));
   free (a);
    

   std::cout <<"+ should be doublefree \n";
   int * b = new int;
   delete b;
   delete b;
   */

   int* a1 = new int;
    a1 = new int;
    a1 = new int;


std::cout <<"del to release array \n";
int* arr = new int [10];
delete arr;

std::cout <<"delete to release malloc \n ";
int * m = (int*) malloc (sizeof(int));
delete m;

std::cout <<"free to release new \n";
int * r = new int;
free (r);

std::cout <<"mem leak \n";
}
