#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <malloc.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <opengm/graphicalmodel/graphicalmodel.hxx>
#include <opengm/graphicalmodel/space/simplediscretespace.hxx>
#include <opengm/functions/potts.hxx>
#include <opengm/operations/adder.hxx>
#include <opengm/inference/messagepassing/messagepassing.hxx>
#include <opengm/inference/gibbs.hxx>

// Define filename etc
#define filename "/home/bernat/Desktop/Projecte/FEATS/0001TP_006690.boosted.txt"
#define rows 880
#define cols 6
using namespace std; // 'using' is used only in example code
using namespace opengm;

// model parameters (global variables are used only in example code)
const size_t nx = 1; // width of the grid
const size_t ny = 880; // height of the grid
const size_t numberOfLabels = 6;
double lambda = 0.1; // coupling strength of the Potts model

// this function maps a node (x, y) in the grid to a unique variable index
inline size_t variableIndex(const size_t x, const size_t y) { 
   return x + nx * y; 
}


main(){
    //readFile();
    float data[880][6];
    std::ifstream file("/home/bernat/Desktop/Projecte/FEATS/0001TP_006690.boosted.txt");

    for(int row = 0; row < 880; ++row) //Saving the data input into a array data.
    {
        std::string line;
        std::getline(file, line);
        if ( !file.good() )
            break;

        std::stringstream iss(line);

        for (int col = 0; col < 6; ++col)
        {
            std::string val;
            std::getline(iss, val, ' ');
            //if ( !iss.good() )
             //   break;

            std::stringstream convertor(val);
            convertor >> data[row][col];
        }
    } //Printing the data
    for (int i =0; i < 880; i++) {

        for (int j = 0; j<6 ;j++) {

        std::cout <<" "<< data[i][j];
       // return 0;
        }
    }
   typedef SimpleDiscreteSpace<size_t, size_t> Space; //We generate the DiscreteSpace [880]
   Space space(nx * ny, numberOfLabels);//([880],6)

   // construct a graphical model with 
   // - addition as the operation (template parameter Adder)
   // - support for Potts functions (template parameter PottsFunction<double>)
   typedef GraphicalModel<double, Adder, OPENGM_TYPELIST_2(ExplicitFunction<double> , PottsFunction<double> ) , Space> Model;
   Model gm(space);
   // for each node (x, y) in the grid, i.e. for each variable 
   // variableIndex(x, y) of the model, add one 1st order functions 
   // and one 1st order factor


//My ExplicitFunction Put values into function?? factor*

   //------------uNARY ENERGIES------------
   //Change limits
   double w0,w1,w2,w3,w4,w5 = 1; //Node weights
   double wa,wb  = 1; //Edge weights
   
   for(size_t y = 0; y < ny; ++y) 
   { //Restored this bucle, i think it's ned for the execution
   for(size_t x = 0; x < nx; ++x) {
      // function
      const size_t shape[] = {numberOfLabels}; //numberOFLabels = 6
      ExplicitFunction<double> f(shape, shape + 1);
      
      //std::cout<<rnd<<","<<rnd_flag<<std::endl;
      
      //i -> 0 to 880
      //j-> 0 - 6
        f(0) = data[y][0] * w0;
        f(1) = data[y][1] * w1;
        f(2) = data[y][2] * w2 ;
      	f(3) = data[y][3] * w3;
      	f(4) = data[y][4] * w4;
      	f(5) = data[y][5] * w5;
      
      

      }

      Model::FunctionIdentifier fid = gm.addFunction(f);
     // std::cout<<f(0)<<","<<f(1)<<"  ";
      // factor
      size_t variableIndices[] = {variableIndex(x,0)};
      gm.addFactor(fid, variableIndices, variableIndices + 1);
   }
   //std::cout<<std::endl;


   //---PAIRWISE ENERGY----------

   //Defining Potts, and doing nothing yet with it
   // add one (!) 2nd order Potts function
   //0 energy when same, lambda energy when different.
   PottsFunction<double> f(numberOfLabels, numberOfLabels, 0.0*wa, lambda*wb);
   Model::FunctionIdentifier fid = gm.addFunction(f);

   // for each pair of nodes (x1, y1), (x2, y2) which are adjacent on the grid,
   // add one factor that connects the corresponding variable indices and 
   // refers to the Potts function


   //Change limits
   for(size_t x = 0; x < nx; ++x) {
      if(x + 1 < nx) { // (x, y) -- (x + 1, y)
         size_t variableIndices[] = {variableIndex(x,0), variableIndex(x + 1,0)};
         sort(variableIndices, variableIndices + 2);
         gm.addFactor(fid, variableIndices, variableIndices + 2);
      }
      
   }   



   //Once model is defined
   //-----INFERENCE---------
   typedef BeliefPropagationUpdateRules<Model, opengm::Minimizer> UpdateRules;
   typedef MessagePassing<Model, opengm::Minimizer, UpdateRules, opengm::MaxDistance> BeliefPropagation;
   const size_t maxNumberOfIterations = 40;
   const double convergenceBound = 1e-7;
   const double damping = 0.5;
   BeliefPropagation::Parameter parameter(maxNumberOfIterations, convergenceBound, damping);
   //End of defining inference method

   //Telling inference method to connect to this graphical model
   BeliefPropagation bp(gm, parameter);
   
   // optimize (approximately)
   BeliefPropagation::VerboseVisitorType visitor;
   bp.infer(visitor); //Actual Optimization


   // obtain the (approximate) argmin
   vector<size_t> labeling(nx * ny);
   bp.arg(labeling); //gives you best labelling : or result of inference.

   /* Labelling = for example
   *
   * 1 --- 2 ---1 --- 3--- 1 ..... so on
   */
   
   // output the (approximate) argmin
   size_t variableIndex = 0;
   for(size_t y = 0; y < ny; ++y) {
      for(size_t x = 0; x < nx; ++x) {
         cout << labeling[variableIndex] << ' ';
         ++variableIndex;
      }   
      cout << endl;
   }





}