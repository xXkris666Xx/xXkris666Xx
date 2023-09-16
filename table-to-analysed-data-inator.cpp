#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<cmath>
#include<vector>

struct Variable {

char name='0';

long double value=0;

long double sigma=0;

long double standard_deviation=0;

long double standard_deviation_mean=0;

};

long double expression_evaluator(std::string);
long double operacao(long double, char, long double);
std::string return_operand_string(std::stringstream&);
long double function_to_double(std::string f, std::vector<Variable> parameters);
long double partial(std::string f, char variable, std::vector<Variable> parameters);
void read_parameters(std::fstream& file, std::vector<Variable>&parameters);
long double uncertainty(std::string f, std::vector<Variable> parameters);
void read_variable(std::fstream& file, std::vector<Variable>& parameters);
void read_txt_table(std::fstream& file, std::vector<Variable>& parameters);
void read_function(std::fstream& file, std::vector<std::string>& functions);
void click_anything();
void receptionist(std::string& filename);
std::string get_edited_table_name(std::string filename);

const std::string euler="2.718281828459045235360287471352";
const std::string pi="3.141592653589793238462643383279";

int main(){
std::string filename;
std::vector<Variable> parameters;
std::vector<std::string> functions;

receptionist(filename);
std::fstream file(filename, std::ios::in);
while(!file.is_open()){
    std::cout<<"\nfile does not exist. ";
    receptionist(filename);
    file.close();
    file.open(filename, std::ios::in);
}

std::string final_file_name=get_edited_table_name(filename);
std::fstream output(final_file_name, std::ios::out);
output<<std::setprecision(40);

read_txt_table(file, parameters);
read_function(file, functions);
for(Variable variavel:parameters){

    output<<variavel.name<<' '<<variavel.value<<" ± "<<variavel.sigma<<'\n';

}

for(std::string f:functions){

output<<'\n'<<f<<"\n\n";
output<<function_to_double(f, parameters)<<" ± "<<uncertainty(f, parameters)<<'\n';

}

file.close();
output.close();
system(final_file_name.c_str());
click_anything();

return 0;
}

long double expression_evaluator(std::string expression)
{
    std::stringstream expressionstream{expression};
    long double operand1, operand2;
    char operador;
        if(expressionstream.peek()=='(')
        {
            operand1=expression_evaluator(return_operand_string(expressionstream));
        } else
        {
            expressionstream>>operand1;
        }

    expressionstream>>operador;

        if(expressionstream.peek()=='(')
        {
            operand2=expression_evaluator(return_operand_string(expressionstream));
        } else
        {
            expressionstream>>operand2;
        }

  return operacao(operand1, operador, operand2);
}

long double operacao(long double operand1, char operador, long double operand2)
{
    if(operador=='+')
    {
        return operand1+operand2;
    }

    if(operador=='-')
    {
        return operand1-operand2;
    }

    if(operador=='*')
    {
        return operand1*operand2;
    }

    if(operador=='/')
    {
        return operand1/operand2;
    }

    if(operador=='^')
    {
        return std::pow(operand1, operand2);
    }

    if(operador=='S')
    {
        return std::sin(operand2);
    }

    if(operador=='C')
    {
        return std::cos(operand2);
    }

    if(operador=='T')
    {
        return std::tan(operand2);
    }

    return 0;
}

std::string return_operand_string(std::stringstream&expressionstream)
{
    std::string operand;
            char c;
            expressionstream>>c;
            int x=1;
            while(true)
            {
                if(x==1 && expressionstream.peek()==')')
                {
                    expressionstream>>c;
                    break;
                }
                expressionstream>>c;
                if(c=='(')
                {
                    ++x;
                }
                if(c==')')
                {
                    --x;
                }
                operand=operand+c;
             }
             return operand;

}

long double function_to_double(std::string f, std::vector<Variable> parameters)
{
    std::string expression;
    for(char c:f){
        bool encontrou=false;

        for(Variable variable:parameters){
            if(variable.name==c)
            {
                std::stringstream ss;
                std::string aux;
                ss<<std::setprecision(400);
                ss<<variable.value;
                ss>>std::setprecision(400);
                ss>>aux;
                expression=expression+aux;
                encontrou=true;
                break;
            }
        }
        if(encontrou==false && c=='e')
        {
            expression=expression+euler;
        }
        else if(encontrou==false && c=='p')
        {
            expression=expression+pi;
        }
        else if(encontrou==false)
        {
            expression=expression+c;
        }

    }
return expression_evaluator(expression);
}


long double partial(std::string f, char variable, std::vector<Variable> parameters)
{
    const static long double h=0.00012207031;

    int i=0;
    for(; i<parameters.size(); ++i){
        if(parameters[i].name==variable)
        {
            break;
        }

    }
    long double auxiliar1, auxiliar2;
    auxiliar1=function_to_double(f, parameters);
    parameters[i].value+=h;
    auxiliar2=function_to_double(f, parameters);
    return (auxiliar2-auxiliar1)/h;

}

void read_parameters(std::fstream& file, std::vector<Variable>&parameters){

char name;
long double value,sigma;
while(file>>name>>value>>sigma)
{
    parameters.push_back({name, value, sigma});
}
return;
}

long double uncertainty(std::string f, std::vector<Variable> parameters){

long double var=0;
for(Variable parameter : parameters){

    var+=parameter.sigma*parameter.sigma*partial(f, parameter.name, parameters)*partial(f, parameter.name, parameters);
}
return std::sqrt(var);
}

void read_variable(std::fstream& file, std::vector<Variable>& parameters){


char name;
long double n=0;
long double equip_sigma, measure, sum=0, sum_squares=0, mean, sigma, standard_deviation=0, standard_deviation_mean=0;
file>>name>>equip_sigma;

while(file>>measure)
{
    if(measure==0)
    {
        break;
    }
    sum+=measure;
    sum_squares+=measure*measure;
    n+=1;
}
mean=sum/n;
if(n==1)
{
    parameters.push_back({name, mean, equip_sigma});
    return;
}

standard_deviation=std::sqrt((sum_squares-mean*mean*n)/(n-1));
standard_deviation_mean=std::sqrt((sum_squares-mean*mean*n)/(n*(n-1)));//already takes sigma of the mean
sigma=std::sqrt(standard_deviation_mean*standard_deviation_mean+equip_sigma*equip_sigma);//accounts for equip uncertainty
parameters.push_back({name, mean, sigma, standard_deviation, standard_deviation_mean});
return;
}

void read_txt_table(std::fstream& file, std::vector<Variable>& parameters){

while(file.peek()!='*'){
    read_variable(file, parameters);

}
return;
}

void read_function(std::fstream& file, std::vector<std::string>& functions){

char star;//we will discart the * character from the 0* sequence
std::string f;
file>>star;
while(file>>f){

functions.push_back(f);

}
return;
}

void click_anything(){

char c;
std::cout<<"\n\nclick any key to leave to leave\n";

std::cin>>std::noskipws>>c;
return;
}

void receptionist(std::string& filename){

std::cout<<"enter file name with table:\n\n";
std::cin>>filename;
return;
}

std::string get_edited_table_name(std::string filename){

std::string new_name;
int len=filename.length();
for(int i=0; i<len; ++i)
{
    if(filename[i]=='.' &&filename[i+1]=='t' &&filename[i+2]=='x' &&filename[i+3]=='t')
    {
        break;
    }
    new_name=new_name+filename[i];
}
return new_name+"_final.txt";
}
