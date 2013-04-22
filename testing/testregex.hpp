#ifndef TESTREGEX_HPP
#define TESTREGEX_HPP


#include <vector>
#include <map>
#include <string>
#include <boost/concept_check.hpp>

#include "testClass.h"
#include <boost/regex.hpp>


class testRegex:public testClass
{
public:
    void test()
    {
	
      
     //http://www.cplusplus.com/reference/regex/ECMAScript/
     
      const boost::regex pattern("\\w+\\(\\w+,\\w+,(\\d+\\.{0,1}\\d*)\\)");
     
      
      std::string str1="CIRCLE(x,y,105)";
      std::string str2="CIRCLE(x,y,10.5)";
      std::string str3="CIRCLE(x,y,a)";
      std::string str4="CIRCLE";
      std::string str5="CIRCLE(x,100)";
      std::string str6="CIRCLE(x,y,10.0.5)";
      std::string str7="CIRCLE()";
      
      assert(boost::regex_match(str1.begin(),str1.end(), pattern)==true);
      assert(boost::regex_match(str2.begin(),str2.end(), pattern)==true);
      assert(boost::regex_match(str3.begin(),str3.end(), pattern)==false);
      assert(boost::regex_match(str4.begin(),str4.end(), pattern)==false);
      assert(boost::regex_match(str5.begin(),str5.end(), pattern)==false);
      assert(boost::regex_match(str6.begin(),str6.end(), pattern)==false);
      assert(boost::regex_match(str7.begin(),str7.end(), pattern)==false);








    }
};

#endif // TESTREGEX_HPP
