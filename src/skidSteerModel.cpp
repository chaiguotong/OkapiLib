/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "okapi/chassis/skidSteerModel.hpp"

namespace okapi {
SkidSteerModelParams::SkidSteerModelParams(const AbstractMotor &ileftSideMotor,
                                           const AbstractMotor &irightSideMotor,
                                           const RotarySensor &ileftEnc,
                                           const RotarySensor &irightEnc)
  : leftSideMotor(ileftSideMotor),
    rightSideMotor(irightSideMotor),
    leftSensor(ileftEnc),
    rightSensor(irightEnc) {
}

SkidSteerModelParams::SkidSteerModelParams(const AbstractMotor &ileftSideMotor,
                                           const AbstractMotor &irightSideMotor)
  : leftSideMotor(ileftSideMotor),
    rightSideMotor(irightSideMotor),
    leftSensor(ileftSideMotor.getEncoder()),
    rightSensor(irightSideMotor.getEncoder()) {
}

SkidSteerModelParams::~SkidSteerModelParams() = default;

std::shared_ptr<const ChassisModel> SkidSteerModelParams::make() const {
  return std::make_shared<const SkidSteerModel>(SkidSteerModel(*this));
}

SkidSteerModel::SkidSteerModel(const AbstractMotor &ileftSideMotor,
                               const AbstractMotor &irightSideMotor, const RotarySensor &ileftEnc,
                               const RotarySensor &irightEnc)
  : leftSideMotor(ileftSideMotor),
    rightSideMotor(irightSideMotor),
    leftSensor(ileftEnc),
    rightSensor(irightEnc) {
}

SkidSteerModel::SkidSteerModel(const AbstractMotor &ileftSideMotor,
                               const AbstractMotor &irightSideMotor)
  : leftSideMotor(ileftSideMotor),
    rightSideMotor(irightSideMotor),
    leftSensor(ileftSideMotor.getEncoder()),
    rightSensor(irightSideMotor.getEncoder()) {
}

SkidSteerModel::SkidSteerModel(const SkidSteerModelParams &iparams)
  : leftSideMotor(iparams.leftSideMotor),
    rightSideMotor(iparams.rightSideMotor),
    leftSensor(iparams.leftSensor),
    rightSensor(iparams.rightSensor) {
}

SkidSteerModel::SkidSteerModel(const SkidSteerModel &other)
  : leftSideMotor(other.leftSideMotor),
    rightSideMotor(other.rightSideMotor),
    leftSensor(other.leftSensor),
    rightSensor(other.rightSensor) {
}

SkidSteerModel::~SkidSteerModel() = default;

void SkidSteerModel::driveForward(const int ipower) const {
  leftSideMotor.moveVelocity(ipower);
  rightSideMotor.moveVelocity(ipower);
}

void SkidSteerModel::driveVector(const int idistPower, const int ianglePower) const {
  leftSideMotor.moveVelocity(idistPower + ianglePower);
  rightSideMotor.moveVelocity(idistPower - ianglePower);
}

void SkidSteerModel::turnClockwise(const int ipower) const {
  leftSideMotor.moveVelocity(ipower);
  rightSideMotor.moveVelocity(-1 * ipower);
}

void SkidSteerModel::stop() const {
  leftSideMotor.moveVelocity(0);
  rightSideMotor.moveVelocity(0);
}

void SkidSteerModel::tank(const int ileftVal, const int irightVal, const int ithreshold) const {
  if (fabs(ileftVal) < ithreshold) {
    leftSideMotor.moveVelocity(0);
  } else {
    leftSideMotor.moveVelocity(ileftVal);
  }

  if (fabs(irightVal) < ithreshold) {
    rightSideMotor.moveVelocity(0);
  } else {
    rightSideMotor.moveVelocity(irightVal);
  }
}

void SkidSteerModel::arcade(int iverticalVal, int ihorizontalVal, const int ithreshold) const {
  if (fabs(iverticalVal) < ithreshold)
    iverticalVal = 0;
  if (fabs(ihorizontalVal) < ithreshold)
    ihorizontalVal = 0;

  leftSideMotor.moveVelocity(iverticalVal + ihorizontalVal);
  rightSideMotor.moveVelocity(iverticalVal - ihorizontalVal);
}

void SkidSteerModel::left(const int ipower) const {
  leftSideMotor.moveVelocity(ipower);
}

void SkidSteerModel::right(const int ipower) const {
  rightSideMotor.moveVelocity(ipower);
}

std::valarray<int> SkidSteerModel::getSensorVals() const {
  return std::valarray<int>{leftSensor.get(), rightSensor.get()};
}

void SkidSteerModel::resetSensors() const {
  leftSensor.reset();
  rightSensor.reset();
}
} // namespace okapi