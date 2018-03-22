/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "okapi/chassis/controller/chassisControllerIntegrated.hpp"
#include "okapi/util/timer.hpp"

namespace okapi {
ChassisControllerIntegrated::ChassisControllerIntegrated(
  const ChassisModelParams &imodelParams,
  const PosIntegratedControllerParams &ileftControllerParams,
  const PosIntegratedControllerParams &irightControllerParams, const double istraightScale,
  const double iturnScale)
  : ChassisController(imodelParams),
    leftController(ileftControllerParams),
    rightController(irightControllerParams),
    lastTarget(0),
    straightScale(istraightScale),
    turnScale(iturnScale) {
}

ChassisControllerIntegrated::ChassisControllerIntegrated(
  std::shared_ptr<const ChassisModel> imodel,
  const PosIntegratedControllerParams &ileftControllerParams,
  const PosIntegratedControllerParams &irightControllerParams, const double istraightScale,
  const double iturnScale)
  : ChassisController(imodel),
    leftController(ileftControllerParams),
    rightController(irightControllerParams),
    lastTarget(0),
    straightScale(istraightScale),
    turnScale(iturnScale) {
}

ChassisControllerIntegrated::ChassisControllerIntegrated(const AbstractMotor &ileftSideMotor,
                                                         const AbstractMotor &irightSideMotor,
                                                         const double istraightScale,
                                                         const double iturnScale)
  : ChassisController(SkidSteerModelParams(ileftSideMotor, irightSideMotor)),
    leftController(ileftSideMotor),
    rightController(irightSideMotor),
    lastTarget(0),
    straightScale(istraightScale),
    turnScale(iturnScale) {
}

ChassisControllerIntegrated::ChassisControllerIntegrated(const AbstractMotor &itopLeftMotor,
                                                         const AbstractMotor &itopRightMotor,
                                                         const AbstractMotor &ibottomRightMotor,
                                                         const AbstractMotor &ibottomLeftMotor,
                                                         const double istraightScale,
                                                         const double iturnScale)
  : ChassisController(
      XDriveModelParams(itopLeftMotor, itopRightMotor, ibottomRightMotor, ibottomLeftMotor)),
    leftController(itopLeftMotor),
    rightController(itopRightMotor),
    lastTarget(0),
    straightScale(istraightScale),
    turnScale(iturnScale) {
}

ChassisControllerIntegrated::~ChassisControllerIntegrated() = default;

void ChassisControllerIntegrated::moveDistance(const int itarget) {
  int distanceElapsed = 0, lastDistance = 0;
  uint32_t prevWakeTime = millis();
  bool atTarget = false;
  const int atTargetDistance = 15;
  const int threshold = 2;

  const auto encStartVals = model->getSensorVals();
  std::valarray<int> encVals{0, 0};

  Timer atTargetTimer;
  const uint32_t timeoutPeriod = 250;

  const double newTarget = (itarget + lastTarget) * straightScale;
  leftController.setTarget(newTarget);
  rightController.setTarget(newTarget);

  while (!atTarget) {
    encVals = model->getSensorVals() - encStartVals;
    distanceElapsed = static_cast<int>((encVals[0] + encVals[1]) / 2.0);

    if (abs(itarget - distanceElapsed) <= atTargetDistance &&
        abs(distanceElapsed - lastDistance) <= threshold)
      atTargetTimer.placeHardMark();
    else
      atTargetTimer.clearHardMark();

    lastDistance = distanceElapsed;

    if (atTargetTimer.getDtFromHardMark() >= timeoutPeriod)
      atTarget = true;

    task_delay_until(&prevWakeTime, 15);
  }
}

void ChassisControllerIntegrated::turnAngle(float idegTarget) {
  lastTarget = 0;
  leftController.reset();
  rightController.reset();

  const double newTarget = idegTarget * turnScale;
  leftController.setTarget(newTarget);
  rightController.setTarget(-1 * newTarget);
}
} // namespace okapi