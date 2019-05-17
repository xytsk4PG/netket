// Copyright 2018 The Simons Foundation, Inc. - All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NETKET_PYQUANTUM_STATE_RECONSTRUCTION_HPP
#define NETKET_PYQUANTUM_STATE_RECONSTRUCTION_HPP

#include <mpi.h>
#include <pybind11/complex.h>
#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <complex>
#include <vector>
#include "quantum_state_reconstruction.hpp"

namespace py = pybind11;

namespace netket {

void AddUnsupervisedModule(py::module &m) {
  auto subm = m.def_submodule("unsupervised");

  py::class_<QuantumStateReconstruction>(subm, "Qsr")
      .def(py::init([](AbstractSampler &sa, AbstractOptimizer &op,
                       int batch_size, int n_samples, py::tuple rotations,
                       std::vector<Eigen::VectorXd> samples,
                       std::vector<int> bases, int discarded_samples,
                       int discarded_samples_on_init, const std::string &method,
                       double diag_shift = 0.01, bool use_iterative = false,
                       bool use_cholesky = true) {
             auto rots = py::cast<std::vector<AbstractOperator *>>(rotations);
             return QuantumStateReconstruction{sa,
                                               op,
                                               batch_size,
                                               n_samples,
                                               std::move(rots),
                                               std::move(samples),
                                               std::move(bases),
                                               discarded_samples,
                                               discarded_samples_on_init,
                                               method,
                                               diag_shift,
                                               use_iterative,
                                               use_cholesky};
           }),
           py::keep_alive<1, 2>(), py::keep_alive<1, 3>(),
           py::keep_alive<1, 7>(), py::arg("sampler"), py::arg("optimizer"),
           py::arg("batch_size"), py::arg("n_samples"), py::arg("rotations"),
           py::arg("samples"), py::arg("bases"),
           py::arg("discarded_samples") = -1,
           py::arg("discarded_samples_on_init") = 0, py::arg("method") = "Gd",
           py::arg("diag_shift") = 0.01, py::arg("use_iterative") = false,
           py::arg("use_cholesky") = true)
      .def("add_observable", &QuantumStateReconstruction::AddObservable,
           py::keep_alive<1, 2>())
      .def("run", &QuantumStateReconstruction::Run, py::arg("output_prefix"),
           py::arg("n_iter"), py::arg("step_size") = 1,
           py::arg("save_params_every") = 50)
      .def("iter", &QuantumStateReconstruction::Iterate,
           py::arg("n_iter") = nonstd::nullopt, py::arg("step_size") = 1, R"EOF(
                      Iterate the optimization of the wavefunction.

                      Args:
                          n_iter: The maximum number of iterations.
                          step_size: Number of iterations performed at a time. Default is
                              1.

                      )EOF")
      .def("advance", &QuantumStateReconstruction::Advance,
           py::arg("steps") = 1,
           R"EOF(
                      Perform one or several iteration steps of the Qsr calculation. In each step,
                      the gradient will be estimated via negative and positive phase and subsequently,
                      the variational parameters will be updated according to the configured method.

                      Args:
                          steps: Number of optimization steps to perform.

                      )EOF")
      .def("get_observable_stats",
           [](QuantumStateReconstruction &self) {
             py::dict data;
             self.ComputeObservables();
             self.GetObsManager().InsertAllStats(data);
             return data;
           },
           R"EOF(
                   Calculate and return the value of the operators stored as observables.

                 )EOF")
      .def("nll", &QuantumStateReconstruction::NegativeLogLikelihood,
           py::arg("rotations"), py::arg("samples"), py::arg("bases"),
           R"EOF(
             Iterate the optimization of the wavefunction.

             Args:
             rotations: Vector of rotations corresponding to the basis rotations.
             samples: Vector of samples.
             bases: Which bases the samples correspond to.

                                 )EOF");
  py::class_<QuantumStateReconstruction::Iterator>(subm, "QsrIterator")
      .def("__iter__", [](QuantumStateReconstruction::Iterator &self) {
        return py::make_iterator(self.begin(), self.end());
      });
}

}  // namespace netket

#endif
