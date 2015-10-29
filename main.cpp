//
//  main.cpp
//  XcodeTest
//
//  A sample SNAP program which uses Xcode.
//
//  For more detailed examples, see the snap/examples directory.

#include "Snap.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>

using namespace TSnap;

class Visitor {
  
private:
  std::vector<int> nodos;
  
public:
  Visitor() { }
  Visitor(const int& Nodes) { }
  void FinishNode(const int& NId) { }
  void ExamineEdge(const int& NId1, const int& NId2) { }
  void TreeEdge(const int& NId1, const int& NId2) { }
  void BackEdge(const int& NId1, const int& NId2) { }
  void FwdEdge(const int& NId1, const int& NId2) { }
  void DiscoverNode(int NId){ nodos.push_back(NId); }
  std::vector<int> getVertices() { return nodos; }
  
};

PUNGraph importarGrafo(TStr nombreArchivo);
std::vector<int> obtenerVerticesOrdenados(PUNGraph G);
void ordenarAsc(std::vector<int>& v, int n);
void grafoGDF(PUNGraph G);
void grafoML(PUNGraph G);
void grafoGEXF(PUNGraph G);
void grafoJSON(PUNGraph G);

int main(int argc, char* argv[]) {
  
  PUNGraph G = importarGrafo("facebook_combined.txt");
  
  auto begin = std::chrono::high_resolution_clock::now();
  grafoGDF(G);
  auto end = std::chrono::high_resolution_clock::now();
  auto gdf = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
  std::cout << "Tiempo para exportar grafo a .gdf: " << gdf.count() << " ms" << std::endl;
  
  begin = std::chrono::high_resolution_clock::now();
  grafoML(G);
  end = std::chrono::high_resolution_clock::now();
  auto gml = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
  std::cout << "Tiempo para exportar grafo a .graphml: " << gml.count() << " ms" << std::endl;
  
  begin = std::chrono::high_resolution_clock::now();
  grafoGEXF(G);
  end = std::chrono::high_resolution_clock::now();
  auto gexf = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
  std::cout << "Tiempo para exportar grafo a .gexf: " << gexf.count() << " ms" << std::endl;
  
  begin = std::chrono::high_resolution_clock::now();
  grafoJSON(G);
  end = std::chrono::high_resolution_clock::now();
  auto json = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
  std::cout << "Tiempo para exportar grafo a .json: " << json.count() << " ms" << std::endl;
  
  
  
  return 0;
}

PUNGraph importarGrafo(TStr nombreArchivo) {
  
  PUNGraph G = TSnap::LoadEdgeList<PUNGraph>(nombreArchivo, 0, 1);
  
  return G;
  
}

std::vector<int> obtenerVerticesOrdenados(PUNGraph G){
  
  Visitor vis(G->GetNodes());
  TCnCom::GetDfsVisitor(G, vis);
  std::vector<int> nodos = vis.getVertices();
  
  ordenarAsc(nodos, int(nodos.size()));
  
  return nodos;
  
}

void ordenarAsc(std::vector<int>& v, int n) {
  
  int index;
  
  for (int i=1; i < n; i++)
  {
    index = v[i];
    
    int j = i-1;
    
    while (j >= 0 && v[j] > index)
    {
      v[j + 1] = v[j];
      j--;
    }
    
    v[j+1] = index;
  }
  
}

void grafoGDF(PUNGraph G) {
  
  std::ofstream myfile;
  std::vector<int> nodos = obtenerVerticesOrdenados(G);
  TIntV conexiones;
  
  myfile.open("facebook.gdf");
  
  myfile << "nodedef>name VARCHAR" << "\n";
  myfile << "edgedef>node1 VARCHAR,node2 VARCHAR" << "\n";
  
  for (int i = 0; i < nodos.size(); i++) {
    
    GetNodesAtHop(G, nodos[i], 1, conexiones, false);
    
    for (int j = 0; j < conexiones.EndI() - conexiones.BegI(); j++) {
      if (conexiones[j] > i) {
        myfile << i << "," << conexiones[j] << "\n";
      }
    }
    
    conexiones.Clr();
    
  }
  
  myfile.close();
  
}//cierre de grafoGDF

void grafoML(PUNGraph G) {
  
  std::ofstream myfile;
  std::vector<int> nodos = obtenerVerticesOrdenados(G);
  long int aristas = 1;
  TIntV conexiones;
  
  myfile.open("facebook.graphml");
  
  myfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";
  myfile << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"" << "\n";
  myfile << "\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << "\n";
  myfile << "\txsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">" << "\n";
  myfile << "  <graph id=\"G\" edgedefault=\"undirected\">" << "\n";
  
  for (int i = 0; i < nodos.size(); i++) {
    
    GetNodesAtHop(G, nodos[i], 1, conexiones, false);
    
    for (int j = 0; j < conexiones.EndI() - conexiones.BegI(); j++) {
      if (conexiones[j] > i) {
        myfile << "  \t<edge id=\"e" << aristas << "\" source=\"" << i << "\" target=\"" << conexiones[j] << "\"/>" << "\n";
        aristas++;
      }
    }
    
    conexiones.Clr();
    
  }
  
  myfile << "  </graph>" << "\n";
  myfile << "</graphml>" << "\n";
  
  myfile.close();
  
}//cierre de grafoGDF

void grafoGEXF(PUNGraph G) {
  
  std::ofstream myfile;
  std::vector<int> nodos = obtenerVerticesOrdenados(G);
  long int aristas = 0;
  TIntV conexiones;
  
  myfile.open("facebook.gexf");
  
  myfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";
  myfile << "<gexf xmlns=\"http://www.gexf.net/1.2draft\" version=\"1.2\">" << "\n";
  myfile << "\t<graph mode=\"static\" defaultedgetype=\"undirected\">" << "\n";
  myfile << "\t\t<edges>" << std::endl;
  
  for (int i = 0; i < nodos.size(); i++) {
    
    GetNodesAtHop(G, nodos[i], 1, conexiones, false);
    
    for (int j = 0; j < conexiones.EndI() - conexiones.BegI(); j++) {
      if (conexiones[j] > i) {
        myfile << "\t\t\t<edge id=\"" << aristas << "\" source=\"" << i << "\" target=\"" << conexiones[j] << "\"/>" << "\n";
        aristas++;
      }
    }
    
    conexiones.Clr();
    
  }
  
  myfile << "\t\t</edges>" << "\n";
  myfile << "\t</graph>" << "\n";
  myfile << "</gexf>" << "\n";
  
  myfile.close();
  
}//cierre de grafoGDF

void grafoJSON(PUNGraph G) {
  
  std::ofstream myfile;
  std::vector<int> nodos = obtenerVerticesOrdenados(G);
  TIntV conexiones;
  
  myfile.open("facebook.json");
  
  
  myfile << "{ \"graph\": {" << "\n";
  myfile << "\"nodes\": [" << "\n";
  
  for (int i = 0; i < nodos.size(); i++) {
    myfile << "{ \"id\": \"" << nodos[i] << "\" }";
    if (i == nodos.size()-1) {
      myfile << " ]," << "\n";
    }
    else {
      myfile << "," << "\n";
    }
  }
  
  myfile << "\"edges\": [\n";
  
  for (int i = 0; i < nodos.size(); i++) {
    
    GetNodesAtHop(G, nodos[i], 1, conexiones, false);
    
    for (int j = 0; j < conexiones.EndI() - conexiones.BegI(); j++) {
      if (conexiones[j] > i) {
        myfile << "{ \"source\": \"" << i << "\", \"target\": \"" << conexiones[j] << "\" }";
        if (i == nodos.size()-1) {
          myfile << " ]" << "\n";
        }
        else {
          myfile << "," << "\n";
        }
      }
    }
    
    conexiones.Clr();
    
  }
  
  myfile << "} }";
  
  myfile.close();
  
  
}












