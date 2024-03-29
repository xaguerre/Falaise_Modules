#include <bayeux/dpp/chain_module.h>
#include <bayeux/mctools/simulated_data.h>
#include <bayeux/mctools/base_step_hit.h>
#include "TFile.h"
#include "TTree.h"
#include "TObject.h"
//#include <ThreeVector.h> 

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
  //:myparam( ps.get<std::string>("outputfile", "Energie_OM.root") )


  void finalize();
  
private:
  // Bayeux' macro to register this class as data processing module
  DPP_MODULE_REGISTRATION_INTERFACE(MyModuleSD);

  double energy_tot;
  std::vector <double> energy_vector;
  std::vector <int> om_id_vector;  
  float vertex_position[3];
  
  TFile *file;
  TTree *Result_tree;

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

void MyModuleSD::initialize(const datatools::properties & parameters, datatools::service_manager &, dpp::module_handle_dict_type &)
{
  std::cout << "+++ MyModuleSD::initialize()" << std::endl;
  
  nb_events_processed = 0; 
  this->_set_initialized(true);

  file = new TFile("Energie_OM.root","RECREATE");
  Result_tree= new TTree("Result_tree","");
  Result_tree->Branch("om_id", &om_id_vector);
  Result_tree->Branch("energy", &energy_vector);
  Result_tree->Branch("energy_tot", &energy_tot);
  Result_tree->Branch("vertex_position", &vertex_position, "vertex_position[3]/F");

}

dpp::chain_module::process_status MyModuleSD::process (datatools::things &event)
{
  //  std::cout << "+++ MyModuleSd::process()" << std::endl;

  double energy_array[712];
  
  memset(energy_array, 0, 712*sizeof(double));

  auto& simData = event.get<mctools::simulated_data>("SD");
  //  simData.tree_dump();
  
  const geomtools::vector_3d & vertex_position_3d = simData.get_vertex();
  vertex_position[0] = vertex_position_3d.x();
  vertex_position[1] = vertex_position_3d.y();
  vertex_position[2] = vertex_position_3d.z();
  if (simData.has_step_hits ("calo")){
    for (auto& a_calo_hit :  simData.get_step_hits("calo")){
      auto& geom_hit =  a_calo_hit->get_geom_id();
      int an_om_id = geom_hit.get(1)*260+geom_hit.get(2)*13 + geom_hit.get(3);
      energy_array[an_om_id] += a_calo_hit->get_energy_deposit();
    }
  }
  if (simData.has_step_hits ("xcalo")){
    for (auto& a_calo_hit :  simData.get_step_hits("xcalo")){
      auto& geom_hit =  a_calo_hit->get_geom_id();
      int an_om_id = 520 + geom_hit.get(1)*64 + geom_hit.get(2)*32  + geom_hit.get(3)*16 + geom_hit.get(4);
      energy_array[an_om_id] += a_calo_hit->get_energy_deposit();
    }
  }
  if (simData.has_step_hits ("gveto")){
    for (auto& a_calo_hit :  simData.get_step_hits("gveto")){
      auto& geom_hit =  a_calo_hit->get_geom_id();
      int an_om_id = 648 + geom_hit.get(1)*32 +geom_hit.get(2)*16+geom_hit.get(3);
      energy_array[an_om_id] += a_calo_hit->get_energy_deposit();
    }
  }

  energy_tot =0;
  energy_vector.clear();
  om_id_vector.clear();
  for (int j =0; j < 712 ; j++){
    if (energy_array[j] > 0){
      energy_tot += energy_array[j];
      //std::cout <<"om_id : " << j << "   energie deposit : " << energy_array[j]<< std::endl;
      energy_vector.push_back (energy_array[j]);
      om_id_vector.push_back (j);
    }
  }


  ++nb_events_processed;
  
  if (energy_vector.size() >0){
    Result_tree->Fill();
    return PROCESS_OK;
  }
  else return PROCESS_STOP;


  
}


void MyModuleSD::finalize()
{
  std::cout << "+++ MyModuleSD::finalize()" << std::endl;
  this->_set_initialized(false);

  file->cd();
  Result_tree->Write("", TObject::kOverwrite);
  file->Close();
}
