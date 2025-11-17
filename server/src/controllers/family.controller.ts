import { Response, NextFunction } from 'express';
import { Family } from '../models/Family.model.js';
import { AuthRequest } from '../middleware/auth.middleware.js';
import { AppError } from '../middleware/errorHandler.js';

export const getFamily = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const family = await Family.findOne({ userId: req.user._id });

    res.json({
      success: true,
      data: family,
    });
  } catch (error) {
    next(error);
  }
};

export const createFamily = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    // Check if family already exists
    const existingFamily = await Family.findOne({ userId: req.user._id });
    if (existingFamily) {
      throw new AppError('Ya tienes un perfil familiar creado', 400);
    }

    const family = await Family.create({
      ...req.body,
      userId: req.user._id,
    });

    res.status(201).json({
      success: true,
      data: family,
    });
  } catch (error) {
    next(error);
  }
};

export const updateFamily = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const family = await Family.findOneAndUpdate(
      { userId: req.user._id },
      req.body,
      { new: true, runValidators: true }
    );

    if (!family) {
      throw new AppError('Perfil familiar no encontrado', 404);
    }

    res.json({
      success: true,
      data: family,
    });
  } catch (error) {
    next(error);
  }
};

export const addMember = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const memberData = req.body;

    // Calculate BMI if weight and height provided
    if (memberData.weight && memberData.height) {
      const heightInMeters = memberData.height / 100;
      memberData.bmi = memberData.weight / (heightInMeters * heightInMeters);
    }

    const family = await Family.findOneAndUpdate(
      { userId: req.user._id },
      { $push: { members: memberData } },
      { new: true, runValidators: true }
    );

    if (!family) {
      throw new AppError('Perfil familiar no encontrado', 404);
    }

    res.json({
      success: true,
      data: family,
    });
  } catch (error) {
    next(error);
  }
};

export const updateMember = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { memberId } = req.params;
    const updateData = req.body;

    // Calculate BMI if weight and height provided
    if (updateData.weight && updateData.height) {
      const heightInMeters = updateData.height / 100;
      updateData.bmi = updateData.weight / (heightInMeters * heightInMeters);
    }

    const family = await Family.findOneAndUpdate(
      { userId: req.user._id, 'members._id': memberId },
      { $set: { 'members.$': { ...updateData, _id: memberId } } },
      { new: true, runValidators: true }
    );

    if (!family) {
      throw new AppError('Miembro no encontrado', 404);
    }

    res.json({
      success: true,
      data: family,
    });
  } catch (error) {
    next(error);
  }
};

export const deleteMember = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { memberId } = req.params;

    const family = await Family.findOneAndUpdate(
      { userId: req.user._id },
      { $pull: { members: { _id: memberId } } },
      { new: true }
    );

    if (!family) {
      throw new AppError('Perfil familiar no encontrado', 404);
    }

    res.json({
      success: true,
      data: family,
    });
  } catch (error) {
    next(error);
  }
};
