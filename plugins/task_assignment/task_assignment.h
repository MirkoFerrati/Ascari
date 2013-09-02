//written by Alessandro Settimi
#ifndef TASK_ASSIGNMENT_H
#define TASK_ASSIGNMENT_H

#include "../HACK_KDEVELOP.h"
#ifdef ISAGENT

#include <vector>

#include <yaml_parser.h>
 
#include "task_assignment_communicator.h"
//#include "assignment_problem.h"
#include <random.hpp>
#include "../shared/objects/task_assignment_task.h"
#include <types/events.h>

#include "../plugins/abstract_agent_plugin.h"
#include "../agent/agent.h"

class task_assignment: public abstract_agent_plugin
{
	
public:
 	task_assignment( const Parsed_World& world, const Parsed_Agent& agent, simulation_time& time, std::map< transition, Events >& events, const std::map< std::string, transition >& events_to_index, const objects_container& tasks );
	task_assignment(agent* a, Parsed_World* parse);
	void createAgentIdAndTaskIdVectorFromParsedWorld(const Parsed_World& wo);
	void createTaskCostMatrixFromParsedWorld(const Parsed_Agent& a);
	void inizializeTaskAssignmentMatrix();
	void run_plugin();
	void addReservedVariables(exprtk::symbol_table< double >& symbol_table);
	void compileExpressions(exprtk::symbol_table< double >& symbol_table);
	void printTaskAssignmentVector();
	void printTaskCostVector();
	void reset_TA_vector();
	void reset_mu_T();
	unsigned int count_undone_task();
	unsigned int count_free_robots();
	void update_remaining_times();
	bool recharge_is_present();
	double dubins_dist(double Ax,double Ay, double theta, double Tx, double Ty, double r);
	//void solve_assignment_problem(assignment_problem& p, task_assignment_namespace::task_assignment_vector& a, std::vector<task_assignment_namespace::task_id>& t,unsigned int n);
	double norm2(double x1,double y1,double x2,double y2);
	void receive_from_others();
	~task_assignment();
	void avoid_collision(double& a);
	void compute_speeds(double x_t, double y_t);
	void createAusiliarVariables();

	
private:
  
	std::map<task_assignment_namespace::agent_id,std::vector<double>> positions;

	double set_charge;
	
	double lambda_u;
	double omega_tilde;
	double beta_p;
	
	std::map<task_assignment_namespace::agent_id,task_assignment_namespace::task_id> taken_tasks;
	std::map<task_assignment_namespace::agent_id,task_assignment_namespace::task_id> previous_taken_tasks;
  
	std::map<task_assignment_namespace::task_id,double> remaining_times_to_deadline;
	
  	rndom<double> rndom_double;
	std::map<task_assignment_namespace::agent_id,bool> busy_robots;
	
	simulation_time& time;
    
	task_assignment_namespace::agent_id my_id;
	double x0,y0;
	
	unsigned int num_robot;
	unsigned int num_task;
	
	task_assignment_namespace::task_id my_task;
	
	double my_task_x,my_task_y;
		
	std::vector<task_assignment_namespace::agent_id> agents_id;
	
	std::vector<task_assignment_namespace::task_id> tasks_id;
	
	task_assignment_namespace::task_cost_matrix task_cost_matrix;
	
	task_assignment_namespace::task_assignment_matrix task_assignment_matrix;
	
	task_assignment_namespace::task_cost_vector base_cost_vector;
	
	task_assignment_namespace::task_cost_vector* agent_task_cost_vector;
	
	task_assignment_namespace::task_assignment_vector* agent_task_assignment_vector;
	
	bool task_assigned;
	bool task_started;
	bool stop;
	
	double speed;
	double omega;
	double omega_dubins;
	double max_omega=0.2;
	exprtk::expression<double> distance_to_target;
	exprtk::expression<double> x;
	exprtk::expression<double> y;
	exprtk::expression<double> theta;
	exprtk::expression<double> charge_;
	std::map< transition, Events >& events;
	const std::map<std::string,transition>& events_to_index;
	
	
	
	task_assignment_communicator_base* ta_communicator;
	
	task_assignment_namespace::task_assignment_algorithm task_assignment_algorithm;
	
	bool convergence_control_routine();
	
	void resolve_assignment_problem();
	
	void update_distance_vector();
	
	double euclidean_distance_from_task(task_assignment_namespace::task_id task_id);
	
	double time_to_deadline(task_assignment_namespace::task_id task_id);
	
	void update_lagrange_multiplier_vector();
	
	//algorithms
	task_assignment_namespace::task_id subgradient_algorithm();
	
		unsigned int passi=1;
		
		task_assignment_namespace::task_id selected_task;
			
		std::vector<double> mu_T;
		
		std::vector<double> subgradient;
		
		std::vector<double> total_subgradient;
				
		std::vector<bool> e_i;
		
		double alpha;
	
		std::vector<double> solution;
	

	//
	
	void setTaskStop(bool stop);
	bool task_made();
	
	std::shared_ptr<std::mutex> ptr_receive_mutex;
	
	bool converge;
	bool fresh_data;
	bool not_started;
	
	unsigned int step;
	
	//algorithms packets ptr

	const objects_container& objects;
	std::map<std::string,task_assignment_task*> tasks;
	std::shared_ptr<task_assignment_namespace::subgradient_packet> ptr_subgradient_packet;
	//
	
	//assignment_problem ta_problem;
	void initialize_assignment_problem();
	std::vector<double> C;
	std::vector<double> D;
	std::vector<double> F;
	std::vector<double> DL;
	void copy_solution_to_TA_vector(std::vector<double>& solution);
	void copy_cost_vector_to_C();
	void control_print();
	void initialize( const Parsed_World& world );

	std::map<task_assignment_namespace::agent_id,std::vector< double >> others_subgradient;
};
	#endif

#endif // TASK_ASSIGNMENT_H
