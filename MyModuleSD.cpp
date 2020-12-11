#include <bayeux/dpp/chain_module.h>
#include <bayeux/mctools/simulated_data.h>
#include <bayeux/mctools/base_step_hit.h>

//////////////////////////////
// Module class declaration //
//////////////////////////////

class MyModuleSD : public dpp::chain_module
{
public:
  MyModuleSD();
  ~MyModuleSD();
  
  void initialize(const datatools::properties &, datatools::service_manager &, dpp::module_handle_dict_type &);
  dpp::chain_module::process_status process(datatools::things &);

  void finalize();
  
private:
  // Bayeux' macro to register this class as data processing module
  DPP_MODULE_REGISTRATION_INTERFACE(MyModuleSD);

  unsigned long long nb_events_processed;
};

/////////////////////////////
// Module class definition //
/////////////////////////////

// Bayeux' macro to register this class as data processing module
DPP_MODULE_REGISTRATION_IMPLEMENT(MyModuleSD, "MyModuleSD");


MyModuleSD::MyModuleSD() : dpp::chain_module() {}

MyModuleSD::~MyModuleSD()
{
  if (this->is_initialized())
    this->finalize();
}

void MyModuleSD::initialize(const datatools::properties &, datatools::service_manager &, dpp::module_handle_dict_type &)
{
  std::cout << "+++ MyModuleSD::initialize()" << std::endl;
  
  nb_events_processed = 0; 
  this->_set_initialized(true);
}

dpp::chain_module::process_status MyModuleSD::process (datatools::things &event)
{
  std::cout << "+++ MyModuleSd::process()" << std::endl;

  auto& simData = event.get<mctools::simulated_data>("SD");
  //  simData.tree_dump();
  
  if (simData.has_step_hits ("calo")){
    for (auto& a_calo_hit :  simData.get_step_hits("calo")){

	
      auto& geom_hit =  a_calo_hit->get_geom_id();
      //geom_hit.get(0);
      // geom_hit.get(1);
      // geom_hit.get(2);
      std::cout << "om_id :  " << geom_hit.get(1)*260+geom_hit.get(2)*13 + geom_hit.get(3)  <<std::endl;

      //a_calo_hit.get_energy_deposit();
      //a_calo_hit.get_time_start();
      
      std::cout << "energy : " << a_calo_hit->get_energy_deposit()  <<std::endl;
      std::cout << "// " <<std::endl;
      
      //}
    }
  }	     

  //  Const hit_handle_collection_type& mctools::simulated_data::get_step_hits ( const std::string &  category_) const
  //  const geomtools::geom_id & get_geom_id () const

  ++nb_events_processed;
  return PROCESS_OK;}
  

void MyModuleSD::finalize()
{
  std::cout << "+++ MyModuleSD::finalize()" << std::endl;
  this->_set_initialized(false);
}
