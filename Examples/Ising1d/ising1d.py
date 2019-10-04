# Copyright 2018 The Simons Foundation, Inc. - All Rights Reserved.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#    http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import netket as nk
import json

# 1D Lattice
g = nk.graph.Hypercube(length=20, n_dim=1, pbc=True)

# Hilbert space of spins on the graph
hi = nk.hilbert.Spin(s=0.5, graph=g)

# Ising spin hamiltonian
ha = nk.operator.Ising(h=1.0, hilbert=hi)

# RBM Spin Machine
ma = nk.machine.RbmSpin(alpha=4, hilbert=hi)
ma.init_random_parameters(seed=1234, sigma=0.01)

ma.max_batch_size = 8

# Metropolis Local Sampling
sa = nk.sampler.MetropolisLocal(machine=ma, batch_size=8, sweep_size=20)

# Optimizer
op = nk.optimizer.Sgd(learning_rate=0.05)

# # Stochastic reconfiguration
# gs = nk.variational.Vmc(
#     hamiltonian=ha,
#     sampler=sa,
#     optimizer=op,
#     n_samples=1000,
#     method="Sr",
#     diag_shift=0.1,
# )
#
# gs.run(output_prefix="test", n_iter=300)
sr = nk.optimizer.SR(diag_shift=0.1)
vmc = nk._vmc_driver.VmcDriver(
    hamiltonian=ha, sampler=sa, optimizer=op, n_samples=1000, sr=sr
)

stats = []
for k in vmc.iter(300):
    stats.append(vmc.get_observable_stats())
    print(k, stats[-1])
