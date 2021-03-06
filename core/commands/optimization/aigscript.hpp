#include <alice/alice.hpp>

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/cut_rewriting.hpp>
#include <mockturtle/algorithms/node_resynthesis.hpp>
#include <mockturtle/algorithms/node_resynthesis/akers.hpp>
#include <mockturtle/algorithms/node_resynthesis/direct.hpp>
#include <mockturtle/algorithms/node_resynthesis/mig_npn.hpp>
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>
#include <mockturtle/algorithms/mig_algebraic_rewriting.hpp>

#include <stdio.h>
#include <fstream>

#include <sys/stat.h>
#include <stdlib.h>

namespace alice
{
  class aigscript_command : public alice::command{

    public:
        explicit aigscript_command( const environment::ptr& env )
                : command( env, "Perform AIG based optimization script" ){

                opts.add_option( "--strategy", strategy, "Optimization strategy [0-4]" );
        }

    protected:
      void execute(){

        if(!store<aig_ntk>().empty()){
          auto& opt = *store<aig_ntk>().current();

          auto start = std::chrono::high_resolution_clock::now();
          mockturtle::depth_view aig_depth{opt};

          //DEPTH REWRITING
          std::cout << "AIG logic depth " << aig_depth.depth() << " nodes " << opt.num_gates() << std::endl;

          switch(strategy){
            default:
            case 0:
            {
              oracle::aig_script aigopt;
              opt = aigopt.run(opt);
            }
            break;
            case 1:
            {
              oracle::aig_script2 aigopt;
              opt = aigopt.run(opt);
            }
            break;
            case 2:
            {
              oracle::aig_script3 aigopt;
              opt = aigopt.run(opt);
            }
            break;
            case 3:
            {
              oracle::aig_script4 aigopt;
              opt = aigopt.run(opt);
            }
            break;
            case 4:
            {
              oracle::aig_script5 aigopt;
              opt = aigopt.run(opt);
            }
            break;
          }
          
          mockturtle::depth_view new_aig_depth{opt};
          std::cout << "AIG logic depth " << new_aig_depth.depth() << " nodes " << opt.num_gates() << std::endl;

          std::cout << "Final ntk size = " << opt.num_gates() << " and depth = " << new_aig_depth.depth() << "\n";
          std::cout << "Area Delay Product = " << opt.num_gates() * new_aig_depth.depth() << "\n";
          auto stop = std::chrono::high_resolution_clock::now();
          auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
          std::cout << "Full Optimization: " << duration.count() << "ms\n";
          std::cout << "Finished optimization\n";

        }
        else{
          std::cout << "There is not an AIG network stored.\n";
        }
        
      }
    private:
        unsigned strategy{0u};
    };

  ALICE_ADD_COMMAND(aigscript, "Optimization");
}