
/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 5                                           *
 * Author: Arash Partow (1999-2012)                           *
 * URL: http://www.partow.net/programming/exprtk/index.html   *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the Common Public License.         *
 * http://www.opensource.org/licenses/cpl1.0.php              *
 *                                                            *
 **************************************************************
*/


#include <cstdio>
#include <string>
#include "exprtk.hpp"

#include <vector>
#include <map>
#include <string>
#include "typedefs.h"
#include "testClass.h"

using namespace std;


template<typename T>
struct myfunc : public exprtk::ifunction<T>
{
   myfunc()
   : exprtk::ifunction<T>(2)
   {}

   inline T operator()(const T& v1, const T& v2)
   {
      return T(1) + (v1 * v2) / T(3);
   }
};

template<typename T>
T custom_function()
{
   typedef exprtk::expression<T> expression_t;
   std::string expression_string = "rndom(1.0,2.0)";
   T x = T(1.0);
   T y = T(2.0);
   myfunc<T> mf;
rndom<T> *rnd=new rndom<T>();
   exprtk::symbol_table<T> symbol_table;
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);
   symbol_table.add_function("myfunc",mf);
   symbol_table.add_function("rndom",*rnd);
   symbol_table.add_constants();

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   exprtk::parser<T> parser;
   parser.compile(expression_string,expression);

   T result = expression.value();
   return result;
}


class testRandom:public testClass
{
public:
    void test() {
		cout<<custom_function<double>()<<endl;
		cout<<custom_function<double>()<<endl;
		cout<<custom_function<double>()<<endl;cout<<custom_function<double>()<<endl;
		cout<<custom_function<double>()<<endl;
		cout<<custom_function<double>()<<endl;
		cout<<custom_function<double>()<<endl;assert(custom_function<double>()<2);
		
	}
};


