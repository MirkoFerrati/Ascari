//written by Alessandro Settimi
#include "bilp_problem.h"

bilp_problem::bilp_problem()
{
  
}

void bilp_problem::initialize_problem(std::string probl_name,int probl_type,int numb_robot, int numb_task,std::vector<double> cost_vector)
{
	problem_name=probl_name;
	problem_type=probl_type;
	num_robot=numb_robot;
	num_task=numb_task;
	
	num_var=num_robot*num_task;
	num_constraints=num_robot+num_task;
	size=num_var*num_constraints; 
	
	initialize_matrices();
	
	create_problem(cost_vector);
}

void bilp_problem::initialize_matrices()
{	
	A_i.push_back(0);	 //negli array usati dal solver l'elemento 0 è riservato al solver
	A_j.push_back(0);
	A_ij.push_back(0);

	double value;
  
	unsigned int z=1;
	int w=0;
	bool app=false;
	
	//std::cout<<std::endl<<"A:"<<std::endl;
	for (int i=0;i<num_constraints;i++)
	{	

		for (int j=0;j<num_var;j++)
		{
			if (i<num_robot)
			{
				if( (j < (i+1)*num_task) && (j > (i*num_task)-1) )  value=1;
				else value=0;
			}			
			else
			{
				while(w<num_robot)
				{
				     app = app || (i-num_robot==j-w*num_task);
				     w++;
				}
				w=0;
				
				if( app )
				{
				    value=1;
				    app=false;
				}
				
				else value=0;
			}
			//std::cout<<value<<' ';
			
			A_ij.push_back(value);; //settaggio dei coefficienti associati alle equazioni di vincolo (A)
			A_i.push_back(i+1); //settaggio indici riga
			A_j.push_back(j+1); //settaggio indici colonna
			z++;
		}
		//std::cout<<std::endl;
	}
	//std::cout<<std::endl;
}


void bilp_problem::add_constraints()
{
	glp_add_rows(problem, num_constraints); //aggiunta delle righe (vincoli)
  
	std::stringstream robot_constraint_name(std::stringstream::out);
	std::stringstream task_constraint_name(std::stringstream::out);
	
	for (int i=1;i<num_robot+1;i++)
	{
		robot_constraint_name << 'r' << i << "_one_task";
		
		glp_set_row_name(problem, i, robot_constraint_name.str().c_str()); //assegnazione nome al vincolo
		glp_set_row_bnds(problem, i, GLP_FX, 1, 1); //settaggio degli estremi nei vincoli
		robot_constraint_name.str("");
		//std::cout<<robot_constraint_name.str()<<std::endl;
	}
	
	unsigned int j=1;
	
	for (int i=num_robot+1;i<num_robot+num_task+1;i++)
	{
		task_constraint_name << 't' << j << "_one_robot";
	
		glp_set_row_name(problem, i, task_constraint_name.str().c_str());
		glp_set_row_bnds(problem, i, GLP_FX, 1, 1);
		task_constraint_name.str("");
		j++;
		//std::cout<<task_constraint_name.str()<<std::endl;
	}
}

void bilp_problem::add_structural_variables()
{
	std::stringstream variable_name(std::stringstream::out);
	
	std::vector<std::string> x;

	for (int i=1;i<num_robot+1;i++)
	{
	    for (int j=1;j<num_task+1;j++)
	    {
		variable_name << 'x' << i << j;
		x.push_back(variable_name.str());
		variable_name.str("");
		//std::cout<<variable_name.str()<<std::endl;
	    }
	}
	
	glp_add_cols(problem, num_var); //aggiunta delle colonne (variabili strutturali) (x)

	for (int i=1;i<num_var+1;i++)
	{
		glp_set_col_name(problem,i,x[i-1].c_str()); //assegnazione nome alla variabile
		glp_set_col_kind(problem,i,GLP_BV); //settaggio del tipo della variabile (binaria)
		//std::cout<<x[i-1]<<std::endl;
	}
}

void bilp_problem::set_cost_vector(std::vector<double> cost_vector)
{
	for (int i=1;i<num_var+1;i++)
	{
		glp_set_obj_coef(problem, i, cost_vector[i-1]); //settaggio del coefficiente del funzionale di costo relativo alla variabile (c)
	}
}

void bilp_problem::create_problem(std::vector<double> cost_vector)
{
	problem = glp_create_prob(); //costruttore, problema vuoto
	glp_set_prob_name(problem, problem_name.c_str()); //assegnazione nome al problema
	glp_set_obj_dir(problem, problem_type); //scelta direzione: min o max
	
	add_constraints();
	
	add_structural_variables();
	
	set_cost_vector(cost_vector);
	
	glp_load_matrix(problem, size, A_i.data(), A_j.data(), A_ij.data()); //caricamento matrice di vincolo (A)

}

void bilp_problem::solve(std::vector<bool>& solution)
{
	std::cout<<">> "<<glp_simplex(problem, NULL)<<" <<"<<std::endl<<std::endl; //metodo del simplesso

	function_value = glp_get_obj_val(problem); //valore del funzionale di costo

	std::cout<<"f:"<<function_value<<std::endl<<std::endl;	

	unsigned int z=1;

	for (int i=0;i<num_robot;i++)
	{
		for (int j=0;j<num_task;j++)
		{
			solution.push_back(glp_get_col_prim(problem, z));  //valore della variabile strutturale (primale)
			z++;
		}
	
	}
	
}


double bilp_problem::get_function_value()
{
	return function_value;
}


bilp_problem::~bilp_problem()
{
	glp_delete_prob(problem); //deallocazione del problema
}
