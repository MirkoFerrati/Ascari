#include "yaml_parser.h"
#include "parserYAML/include/yaml-cpp/node.h"
#include "../plugins/task_assignment/task_assignment_parser_plugin.h"
#include "../plugins/agent_router/agent_router_parser_plugin.h"
#include "../plugins/monitor/monitor_parser_plugin.h"

using namespace std;

yaml_parser::yaml_parser ( std::vector< abstract_parser_plugin* > plugins ) :plugins ( plugins )
{

}

yaml_parser::yaml_parser()
{

}


void yaml_parser::addPlugin ( abstract_parser_plugin* plugin )
{
    assert ( plugin->isCreated ); //will give segfault if the plugin was not created with a new()
    plugins.push_back ( plugin );
}

Parsed_World yaml_parser::parse_file ( const char * file_name )
{
    std::string temp ( file_name );
    return parse_file ( temp );
}


Parsed_World yaml_parser::parse_file ( string file_name )
{
    std::ifstream fin ( file_name.c_str() );
    if ( !fin.is_open() )
        ERR ( "ATTENZIONE, impossibile aprire il file %s",file_name.c_str() );
    std::ostringstream str;
    str<<fin.rdbuf();
    std::string new_str=str.str();
    boost::algorithm::to_upper ( new_str );
    std::stringstream new_new_str;
    new_new_str<<new_str;
    YAML::Parser parser ( new_new_str );
    YAML::Node doc;
    parser.GetNextDocument ( doc );
    Parsed_World World;
    World.parsedSuccessfully=true;
    World.plugins=plugins;

    if (doc.FindValue("REQUIRED_PLUGINS"))
    {
	std::vector<std::string> temp;
	doc["REQUIRED_PLUGINS"]>>temp;
	for (auto plugin_name:temp)
	{
	  bool found=false;
	  for (auto plugin:plugins)
	  {
	      if (plugin->getType()==plugin_name)
		found=true;
	  }
	  if (!found)
	  {
	    ERR("required plugin %s not found,maybe it was not included in this executable?",plugin_name.c_str());
	    World.parsedSuccessfully=false;
	    return World;
	  }
	}
    }
    
    if ( ! ( World.load_from_node ( doc ) ) )
        World.parsedSuccessfully=false;

    return World;

}


bool Parsed_Behavior::load_from_node ( const YAML::Node& node )
{
    node["STATES"]>>state;//behavior->state;
    node["CONTROL_COMMANDS"]>>inputs;//behavior->inputs;
    node["NAME"]>>name;//behavior->name;

    for ( unsigned int i=0; i<state.size(); i++ ) //behavior->state.size(); i++ )
    {
        dynamic_expression tmp_exp;
        //node["DYNAMIC_MAP"][0][behavior->state[i]]>>tmp_exp;
        node["DYNAMIC_MAP"][0][state[i]]>>tmp_exp;
        //behavior->
        expressions.insert ( std::pair<stateVariable,dynamic_expression> ( state[i],tmp_exp ) );
    }

    const YAML::Node& control = node["CONTROLLERS"];
    string temp;
    for ( unsigned int i=0; i<control.size(); i++ )
    {
        map<int,string> temp1;
        control[i]["NAME"]>>temp;
        //behavior->
        controllers.insert ( pair<controller_name, map<int,controllerRule> > ( temp,temp1 ) );
        for ( unsigned int j=0; j<inputs.size(); j++ )
        {
            control[i][inputs[j]]>> controllers[temp][j];
        }
    }

    const YAML::Node& disc_states = node["DISCRETE_STATES"];
    for ( unsigned int i=0; i<disc_states.size(); i++ )
    {
        string n_state;
        disc_states[i]["NAME"]>>n_state;
        string c_name;
        disc_states[i]["CONTROLLER"]>>c_name;
        discrete_states.insert ( pair<string,string> ( n_state,c_name ) );
    }

    const YAML::Node& encoder = node["ENCODER"][0];
    if ( encoder.FindValue ( "TOPOLOGY" ) )
    {
        const YAML::Node& topo=encoder["TOPOLOGY"][0];
        topo["TOPOLOGY"]>> topology;
        for ( unsigned int i=0; i<topology.size(); i++ )
        {
            string topology_exp;
            topo[topology[i]]>>topology_exp;
            topology_expressions.insert ( pair<string,string> ( topology[i],topology_exp ) );
        }
    }

    if ( encoder.FindValue ( "LAMBDA" ) )
    {
        const YAML::Node& lam=encoder["LAMBDA"][0];
        lam["LAMBDA"]>> lambda;
        for ( unsigned int i=0; i<lambda.size(); i++ )
        {
            string lambda_exp;
            lam[lambda[i]]>>lambda_exp;
            lambda_expressions.insert ( pair<string,string> ( lambda[i],lambda_exp ) );
        }
    }

    const YAML::Node& decoder = node["EVENTS"][0];
    decoder["EVENTS"]>> events;

    for ( unsigned int i=0; i<events.size(); i++ )
    {
        string events_exp;
        decoder[events[i]]>>events_exp;
        events_expressions.insert ( pair<string,string> ( events[i],events_exp ) );
    }

    const YAML::Node& automa = node["AUTOMATON"];

    for ( unsigned int i=0; i<automa.size(); i++ )
    {
        for ( map< discreteState_Name, controller_name >::const_iterator iter=discrete_states.begin(); iter!=discrete_states.end(); ++iter )
        {
            string actual_state;
            actual_state= ( ( *iter ).first );
            if ( automa[i].FindValue ( actual_state ) )
            {
                map<string,string> temp1;
                automaton.insert ( pair<string, map<string,string> > ( actual_state,temp1 ) );
                const YAML::Node& transition = automa[i][actual_state][0];
                for ( map< discreteState_Name, controller_name >::const_iterator iiter=discrete_states.begin(); iiter!=discrete_states.end(); ++iiter )
                {
                    string new_state;
                    new_state= ( *iiter ).first;
                    if ( transition.FindValue ( new_state ) )
                    {
                        vector<event_name> tran_ev;
                        transition[new_state]>> tran_ev;
                        for ( unsigned int j=0; j< tran_ev.size(); j++ )
                        {
                            if ( automaton[actual_state].count ( tran_ev[j] ) )
                            {
                                ERR ( "DUPLICATED EVENT %s", tran_ev[j].c_str() );
                                return false;
                            }
                            automaton[actual_state].insert ( pair<event_name, discreteState_Name> ( tran_ev[j],new_state ) );
                        }
                    }
                }
            }
        }
    }
    return true;
}




bool Parsed_Agent::load_from_node ( const YAML::Node& node )
{
    if ( node.FindValue ( "VISIBLE_AREA" ) )
    {
        node["VISIBLE_AREA"]>>visibility;
    }
    else
    {
        WARN ( "NO VISIBLE AREA SPECIFIED",NULL );
    }
     if ( node.FindValue ( "SIMULATED" ) )
    {
	int tmp_simulated;
        
	
	node["SIMULATED"]>>tmp_simulated;
	
	if (tmp_simulated==1)
	  simulated=true;
	else
	{
	  simulated=false;
		
	if ( node.FindValue ( "MARKER" ) )
      {
	node["MARKER"]>>marker;
      }
      else
      {
	ERR ( "AGENT REAL BUT NO MARKER SPECIFIED ");
	return false;
      }
	}
    }
    
    node["COMMUNICATION_AREA"]>>communication;

    for ( unsigned int i=0; i<behavior->state.size(); i++ )
    {
        initial_state_value tmp_initial=0;
        node["INITIAL"][0][behavior->state[i]]>>tmp_initial;
        initial_states.insert ( pair<stateVariable,initial_state_value> ( behavior->state[i],tmp_initial ) );
    }

    node["STATE_START"]>> state_start;

    if ( !behavior->discrete_states.count ( state_start ) )
    {
        ERR ( "UNDEFINED START DISCRETE STATE %s", state_start.c_str() );
        return false;
    }
    return true;
}



bool Parsed_World::load_from_node ( const YAML::Node& node )
{
    mapfilename="UNSET";

    if ( node[0].FindValue ( "WORLD" ) )
    {
        if ( node[0]["WORLD"].size() >0 )
        {

            if ( node[0]["WORLD"][0].FindValue ( "BONUS_VARIABLES" ) )
            {
                const YAML::Node &world_node=node[0]["WORLD"][0];
                world_node["BONUS_VARIABLES"]>> bonus_variables;
                for ( unsigned int i=0; i<bonus_variables.size(); i++ )
                {
                    string bonus_exp;
                    world_node[bonus_variables[i]]>>bonus_exp;
                    bonus_expressions.insert ( make_pair ( bonus_variables.at ( i ),bonus_exp ) );
                }
            }
            if ( node[0]["WORLD"][0].FindValue ( "WORLD_MAP" ) )
            {
                const YAML::Node &world_node=node[0]["WORLD"][0];
                world_node["WORLD_MAP"]>> mapfilename;
                boost::algorithm::to_lower ( mapfilename );
            }
        }
    }

for ( auto plugin:plugins )
    {
        abstract_parsed_world_plugin* temp=0;
	temp=plugin->parseWorld ( node );
        if ( temp!=0 )
            parsed_items_from_plugins.push_back (temp  );
    }

    const YAML::Node &behaviors_nodes=node[0]["BEHAVIORS"];

    for ( unsigned int i=0; i<behaviors_nodes.size(); i++ )
    {
        std::string tmp_beh_name;
        if ( !behaviors_nodes[i].FindValue ( "NAME" ) )
        {
            ERR ( "BEHAVIOR NAME UNDEFINED: BEHAVIOR NUMBER %d", i );
            return false;
        }
        behaviors_nodes[i]["NAME"]>> tmp_beh_name;
        if ( behaviors.find ( tmp_beh_name ) !=behaviors.end() )
        {
            ERR ( "DUPLICATED BEHAVIOR %s", tmp_beh_name.c_str() );
            return false;
        }
        std::unique_ptr<Parsed_Behavior> tmp_ptr ( new Parsed_Behavior() );
        behaviors.insert ( std::make_pair ( tmp_beh_name,std::move ( tmp_ptr ) ) );
        behaviors[tmp_beh_name]->name=tmp_beh_name;
        if ( !  behaviors[tmp_beh_name]->load_from_node ( behaviors_nodes[i] ) )
            return false;
    }


    const YAML::Node &agent_nodes=node[0]["AGENTS"];
    for ( unsigned int i=0; i<agent_nodes.size(); i++ )
    {
        std::string tmp_ag_name;
        agent_nodes[i]["AGENT"]>>tmp_ag_name;
        if ( !agent_nodes[i].FindValue ( "BEHAVIOR" ) )
        {
            ERR ( "BEHAVIOR NAME UNSPECIFIED FOR AGENT %s", tmp_ag_name.c_str() );
            return false;
        }
        std::string tmp_agent_behavior_name;
        agent_nodes[i]["BEHAVIOR"]>>tmp_agent_behavior_name;
        if ( !behaviors.count ( tmp_agent_behavior_name ) )
        {
            ERR ( "UNDEFINED BEHAVIOR %s FOR AGENT %s", tmp_agent_behavior_name.c_str(), tmp_ag_name.c_str() );
            return false;
        }
        agents.emplace_back ( behaviors[tmp_agent_behavior_name] );
        agents.back().name=tmp_ag_name;
        agents.back().behavior_name=tmp_agent_behavior_name;
        if ( ! ( agents.back().load_from_node ( agent_nodes[i] ) ) )
            return false;
    for ( auto plugin:plugins )
        {
            abstract_parsed_agent_plugin* temp_ptr=0;
	    temp_ptr=plugin->parseAgent ( agent_nodes[i] );
            if ( temp_ptr!=0 )
	    {
                agents.back().parsed_items_from_plugins.push_back (temp_ptr);
	    }
        }
    }
    return true;
}
