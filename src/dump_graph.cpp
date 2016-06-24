#include "dump_graph.hpp"
#include <cppformat/format.h>
#include <unordered_set>

#include "buffer_node.hpp"
#include "cast_node.hpp"
#include "clear_node.hpp"
#include "compute_node.hpp"
#include "draw_node.hpp"
#include "dyn_cast.hpp"
#include "image_node.hpp"

void dump_node_info(std::ostream &out, const node &n) {
  // Nxxx [label="{}"]
  /*const char *ty = "?";
  if (auto clearNode = dyn_cast<const clear_node>(&n)) {
    ty = "clear_node";
  } else if (auto imageNode = dyn_cast<const image_node>(&n)) {
    ty = "image_node";
  } else {
    // unknown
  }

  fmt::print(out, "N{:03} [label=\"{{ {} }}\"];\n", n.uid_, ty);*/
}

void dump_value_info(std::ostream &out, const value_impl &v) {
  /*const char *ty = "?";
  if (auto imageVal = dyn_cast<const image_impl>(&v)) {
    ty = "image";
  } else if (auto bufVal = dyn_cast<const buffer_impl>(&v)) {
    ty = "buffer";
  }
  fmt::print(out, "E{:03} [label=\"{{ {} }}\"];\n", v.uid_, ty);*/
}

void dump_graph_dot(std::vector<node *> roots, std::ostream &out) {
  /*std::vector<node *> visiting;
  std::unordered_set<node *> visited;
  std::vector<node *> to_visit;

  visiting = roots;

  // for each node in copy of to_visit
  // - iterate over values
  // - for each value
  // -- print all successor node ids
  // -- add nodes to to_visit

  fmt::print(out, "digraph G {{\nedge [label=0];\ngraph [ranksep=0];\nnode "
                  "[shape=record];\n ");

  auto add_node = [&visited, &to_visit](node &n) {
    if (!visited.count(&n)) {
      to_visit.push_back(&n);
      visited.insert(&n);
    }
  };

  if (!visiting.empty()) {
    for (auto n : visiting) {
      dump_node_info(out, *n);
	  // specify return type as a workaround for VS bug 
	  // http://stackoverflow.com/questions/34248067/msvc-stdfunction-doesnt-accept-generic-lambda-with-a-nested-lambda
      n->traverse([&out, n, &add_node](auto &&edge) -> void {
        dump_value_info(out, edge);
        fmt::print(out, "N{:03} -> E{:03} [label={}];\n", n->uid_, edge.uid_,
                   edge.port_id_);
        edge.traverse([&out, &edge, &add_node](auto &&node) {
          fmt::print(out, "E{:03} -> N{:03};\n", node.uid_, edge.uid_);
          add_node(node);
        });
      });
    }
    visiting = to_visit;
    to_visit.clear();
  }

  fmt::print(out, "}}\n");*/
}
