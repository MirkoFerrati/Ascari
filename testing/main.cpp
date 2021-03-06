
#include "testdynamic.hpp"
#include "testClass.h"
#include "testrandom.hpp"
 #include "time.h"
#include "testserialize.hpp"
#include "testregex.hpp"
#include "testvisibility.hpp"
//#include "testobjectserialization.hpp"
// #include "testgeometry.hpp"
#include <forward_list>
#include <logog.hpp>
/**
 * @author MirkoF
 * Questo progetto e' ancora troppo piccolo per avere un vero framework di unit testing, per adesso
 * basta e avanza un sistema banale come quello qui sotto: una classe base di test e tutti i test
 * che derivano da quella, e un vettore con i test aggiunti manualmente.
 */

int main ( int /*argc*/, char** /*argv*/ )
{


    srand ( time ( NULL ) );
    LOGOG_INITIALIZE();
    {
        logog::Cout out;

        int i=0;
        std::forward_list<double> test;
        for ( i=0; i<10; i++ )
            test.insert_after ( test.before_begin(),i );
        auto old_pointer=test.before_begin();
        for ( auto pointer=test.begin(); pointer!=test.end(); ++pointer )
        {
            old_pointer++;
            if ( true ) //(*pointer>0)
            {
                test.erase_after ( old_pointer );
                pointer=old_pointer;
                break;
            }


        }

    for ( auto p:test )
            cout<<p<<" ";
        cout<<endl;



        vector<pair<testClass*,string> > tests;
        tests.push_back ( make_pair ( new testRandom(),"random" ) );
        tests.push_back ( make_pair ( new testDynamic(),"dinamica" ) );
        tests.push_back ( make_pair ( new testSerialize(),"Serialize" ) );
        tests.push_back ( make_pair ( new testRegex(),"Regex" ) );
        tests.push_back ( make_pair ( new testVisibility(),"Visibility" ) );


        for ( unsigned int i=0; i<tests.size(); i++ )
        {
            try
            {
                cout<<"inizio test "<<tests[i].second<<endl;
                tests[i].first->test();
                cout<<"il test "<<tests[i].second<<" e' andato a buon fine"<<endl;
            }
            catch ( const char* e )
            {
                cout<<e<<endl;
            }
            catch ( ... )
            {
                cout<<"il test "<<tests[i].second<<" e' fallito"<<endl;
                throw;
            }
        }

        for ( unsigned int i=0; i<tests.size(); i++ )
        {
            delete ( tests.at ( i ).first );
        }
    }
}



