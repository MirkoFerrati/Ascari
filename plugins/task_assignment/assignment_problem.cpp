//written by Alessandro Settimi
#include "assignment_problem.h"
#include <iostream>


assignment_problem::assignment_problem()
{
	problem=NULL;
	
	problem_name="";
	problem_type=0;
	
	num_task=0;
	
	num_var=0;
	num_constraints=0;

	function_value=0;
}

void assignment_problem::initialize_problem(std::string probl_name,int probl_type, int numb_task,std::vector<double>& cost_vector)
{
	problem_name=probl_name;
	problem_type=probl_type;
	num_task=numb_task;
	
	num_var=num_task;
	num_constraints=num_task; 
	
	initialize_matrices();
	
	create_problem(cost_vector);
}

void assignment_problem::initialize_matrices()
{	
	A_i.push_back(0);	 //negli array usati dal solver l'elemento 0 e' riservato al solver
	A_j.push_back(0);
	A_ij.push_back(0);

	unsigned int z=1;
	int i=0;
	
	//std::cout<<std::endl<<"A:"<<std::endl;
	for (int j=0;j<num_var;j++)
	{	
		A_ij.push_back(1);; //settaggio dei coefficienti associati alle equazioni di vincolo (A)
		A_i.push_back(i+1); //settaggio indici riga
		A_j.push_back(j+1); //settaggio indici colonna
		z++;
		
		//std::cout<<"A("<<A_i.at(j+1)<<','<<A_j.at(j+1)<<") = "<< A_ij.at(j+1)<<std::endl;
	}
	//std::cout<<std::endl;

}


void assignment_problem::add_constraints()
{
	glp_add_rows(problem, num_constraints); //aggiunta delle righe (vincoli)
  
	std::stringstream robot_constraint_name(std::stringstream::out);
	
	robot_constraint_name << 'r' << "_one_task";
	
	glp_set_row_name(problem, 1, robot_constraint_name.str().c_str()); //assegnazione nome al vincolo
	glp_set_row_bnds(problem, 1, GLP_UP, 1, 1); //settaggio degli estremi nei vincoli
	//std::cout<<robot_constraint_name.str()<<std::endl;
	robot_constraint_name.str("");
}

void assignment_problem::add_structural_variables()
{
	std::stringstream variable_name(std::stringstream::out);
	
	std::vector<std::string> x;

	for (int j=1;j<num_task+1;j++)
	{
	    variable_name << 'x' << j;
	    x.push_back(variable_name.str());
	    //std::cout<<variable_name.str()<<std::endl;
	    variable_name.str("");
	}
	
	glp_add_cols(problem, num_var); //aggiunta delle colonne (variabili strutturali) (x)

	for (int i=1;i<num_var+1;i++)
	{
		glp_set_col_name(problem,i,x.at(i-1).c_str()); //assegnazione nome alla variabile
		glp_set_col_kind(problem,i,GLP_CV); //settaggio del tipo della variabile (continua)
		glp_set_col_bnds(problem,i,GLP_DB,0,1); //settaggio intervallo appartenzenza variabile
		//std::cout<<x[i-1]<<std::endl;
	}
}

void assignment_problem::set_cost_vector(std::vector<double>& cost_vector)
{
	for (int i=1;i<num_var+1;i++)
	{
		glp_set_obj_coef(problem, i, cost_vector.at(i-1)); //settaggio del coefficiente del funzionale di costo relativo alla variabile (c)
	}
}

void assignment_problem::get_cost_vector(std::vector<double>& cost_vector)
{
	for (int i=1;i<num_var+1;i++)
	{
		cost_vector.at(i-1) = glp_get_obj_coef(problem, i);
	}
}


void assignment_problem::create_problem(std::vector<double>& cost_vector)
{
	problem = glp_create_prob(); //costruttore, problema vuoto
	glp_set_prob_name(problem, problem_name.c_str()); //assegnazione nome al problema
	glp_set_obj_dir(problem, problem_type); //scelta direzione: min o max
	
	add_constraints();
	
	add_structural_variables();
	
	set_cost_vector(cost_vector);
	
	glp_load_matrix(problem, num_task, A_i.data(), A_j.data(), A_ij.data()); //caricamento matrice di vincolo (A)
}

void assignment_problem::solve(std::vector<double>& solution)
{
	//std::cout<<">> "<<glp_simplex(problem, NULL)<<" <<"<<std::endl<<std::endl; //metodo del simplesso

	glp_simplex(problem, NULL);
	
	function_value = glp_get_obj_val(problem); //valore del funzionale di costo

	//std::cout<<"f:"<<function_value<<std::endl<<std::endl;	

	unsigned int z=1;

	for (int j=0;j<num_task;j++)
	{
		solution.push_back(glp_get_col_prim(problem, z));  //valore della variabile strutturale (primale)
		//std::cout<<solution.at(j);
		z++;
	}
	std::cout<<std::endl;
}


double assignment_problem::get_function_value()
{
	return function_value;
}


assignment_problem::~assignment_problem()
{
	glp_delete_prob(problem); //deallocazione del problema
}
