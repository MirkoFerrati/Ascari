#include "encoderDet.h"
#include <logog.hpp>


using namespace std;


encoderDet::encoderDet(std::map< int, sub_event_value >& sub_events, const string& agent_name, agent_state& state, const std::map< string,
                       int >& stateVariablesName_to_Index,  std::map< int, double >& bonusVariables,
                       const std::map< string, int >& bonusVariablesName_to_Index, const map< string, string >& topology_expres, const index_map& sub_events_to_index,
                       const map< string, string >& lambda_expres, exprtk::symbol_table<double>& symbol_table)
        :ref_sub_events(sub_events),symbol_table(symbol_table),agent_name(agent_name)

{



//crea espressioni topologie e lambda rispettando gli indici

    for (map<string,int>::const_iterator it=stateVariablesName_to_Index.begin();it!=stateVariablesName_to_Index.end();++it)
    {
        symbol_table.add_variable(it->first,state.at(it->second));
        state_target.insert(make_pair(it->second,0));
        symbol_table.add_variable(it->first+"i",state_target.at(it->second));
    }

    for (map<string,int>::const_iterator it=bonusVariablesName_to_Index.begin();it!=bonusVariablesName_to_Index.end();++it)
    {
        symbol_table.add_variable(it->first,bonusVariables.at(it->second));
    }
    symbol_table.add_constants();

	
    exprtk::parser<double> parser;

    for (map<string,string>::const_iterator it=lambda_expres.begin();it!=lambda_expres.end();++it) {

        exprtk::expression<double> tmp_expression;
        tmp_expression.register_symbol_table(symbol_table);
        string string_tmp_expression=it->second;
        parser.compile(string_tmp_expression,tmp_expression);
        lambda_expressions.insert(make_pair(sub_events_to_index.at(it->first),tmp_expression));
    }


    for (map<string,string>::const_iterator it=topology_expres.begin();it!=topology_expres.end();++it) {

        exprtk::expression<double> tmp_expression;
        tmp_expression.register_symbol_table(symbol_table);
        string string_tmp_expression=it->second;
        if (parser.compile(string_tmp_expression,tmp_expression))
			topology_expressions.insert(make_pair(sub_events_to_index.at(it->first),tmp_expression));
		else
		{
			ERR("Errore durante la creazione dell'espressione della topologia %s,:%s",string_tmp_expression.c_str(),parser.error().c_str());
			throw "Errore durante la creazione dell'espressione della topologia";
		}
    }

}


// encoderDet::~encoderDet()
// {
// 	symbol_table.~symbol_table();
// }


void encoderDet::computeSubEvents(const map< string, agent_state_packet >& state_other_agents)
{
    for (map<int,sub_event_value>::iterator it=ref_sub_events.begin();it!=ref_sub_events.end();++it) {
        it->second=_FALSE;
    }

    for (map<int,exprtk::expression<double> >::const_iterator it=lambda_expressions.begin();it!=lambda_expressions.end();++it)
    {
        if (it->second.value()==1)
            ref_sub_events.at(it->first)=_TRUE;
        else
            ref_sub_events.at(it->first)=_FALSE;
    }

    for (map< string, agent_state_packet >::const_iterator it=state_other_agents.begin();it!=state_other_agents.end();++it) {

        if (it->first!=agent_name)
        {
            for (map_int_double::const_iterator iit=(*it).second.state.begin();iit!=(*it).second.state.end();++iit)
            {
                state_target.at(iit->first)=iit->second;
            }

            for (map<int,exprtk::expression<double> >::const_iterator iit=topology_expressions.begin();iit!=topology_expressions.end();++iit)
            {
                if (ref_sub_events.at(iit->first)==_FALSE) {
                    if (iit->second.value()==1)
                        ref_sub_events.at(iit->first)=_TRUE;
                    else
                        ref_sub_events.at(iit->first)=_FALSE;
                }
            }

        }

    }

}
