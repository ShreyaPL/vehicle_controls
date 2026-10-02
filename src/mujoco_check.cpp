#include <cmath>
#include <iostream>
#include <memory>
#include <mujoco/mujoco.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: mujoco_check model.xml\n";
    return 2;
  }
  if (mj_version() != mjVERSION_HEADER) {
    return 1;
  }
  char error[1024]{};
  const std::unique_ptr<mjModel, decltype(&mj_deleteModel)> model(
      mj_loadXML(argv[1], nullptr, error, sizeof(error)), mj_deleteModel);
  if (!model) {
    std::cerr << error << '\n';
    return 1;
  }
  const std::unique_ptr<mjData, decltype(&mj_deleteData)> data(mj_makeData(model.get()),
                                                               mj_deleteData);
  if (!data || model->nq != 4 || model->nu != 0) {
    return 1;
  }
  for (int tick = 0; tick < 1000; ++tick) {
    mj_step(model.get(), data.get());
    for (int i = 0; i < model->nq; ++i) {
      if (!std::isfinite(data->qpos[i])) {
        return 1;
      }
    }
  }
  for (int i = 0; i < mjNWARNING; ++i) {
    if (data->warning[i].number != 0) {
      return 1;
    }
  }
  std::cout << "MuJoCo " << mj_versionString() << ": articulated scene loaded and stepped for "
            << data->time << " seconds\n";
  return 0;
}
