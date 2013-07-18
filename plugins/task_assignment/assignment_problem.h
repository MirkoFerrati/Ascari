//written by Alessandro Settimi


#include <glpk.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>

class assignment_problem
{
  
public:
    assignment_problem();
    void initialize_problem(std::string problem_name,int problem_type,int num_task,std::vector<double>& cost_vector);
    void initialize_matrices();
    void add_constraints();
    void add_structural_variables();
    void create_problem(std::vector<double>& cost_vector);
    void set_cost_vector(std::vector<double>& cost_vector);
    void get_cost_vector(std::vector<double>& cost_vector);
    void solve(std::vector<double>&);
    double get_function_value();
    
    ~assignment_problem();
  
private:
    glp_prob *problem; //puntatore a problema
    
    std::string problem_name;
    int problem_type;
    
    int num_task;
    
    int num_var;
    int num_constraints;

    std::vector<int> A_i;
    std::vector<int> A_j;
    std::vector<double> A_ij;
    double function_value;
};