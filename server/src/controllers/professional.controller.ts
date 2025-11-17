import { Request, Response, NextFunction } from 'express';
import { Professional } from '../models/Professional.model.js';
import { AuthRequest } from '../middleware/auth.middleware.js';
import { AppError } from '../middleware/errorHandler.js';

export const getProfessionals = async (
  req: Request,
  res: Response,
  next: NextFunction
) => {
  try {
    const { specialization, minRating, search } = req.query;

    const filter: any = { isVerified: true };

    if (specialization) {
      filter.specialization = { $in: specialization };
    }

    if (minRating) {
      filter.rating = { $gte: Number(minRating) };
    }

    const professionals = await Professional.find(filter)
      .populate('userId', 'name email avatar')
      .sort({ rating: -1 });

    res.json({
      success: true,
      data: professionals,
    });
  } catch (error) {
    next(error);
  }
};

export const getProfessional = async (
  req: Request,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const professional = await Professional.findById(id).populate(
      'userId',
      'name email avatar'
    );

    if (!professional) {
      throw new AppError('Profesional no encontrado', 404);
    }

    res.json({
      success: true,
      data: professional,
    });
  } catch (error) {
    next(error);
  }
};

export const createProfessionalProfile = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    // Check if profile already exists
    const existingProfile = await Professional.findOne({ userId: req.user._id });
    if (existingProfile) {
      throw new AppError('Ya tienes un perfil profesional creado', 400);
    }

    const professional = await Professional.create({
      ...req.body,
      userId: req.user._id,
    });

    res.status(201).json({
      success: true,
      data: professional,
    });
  } catch (error) {
    next(error);
  }
};

export const updateProfessionalProfile = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const professional = await Professional.findOneAndUpdate(
      { userId: req.user._id },
      req.body,
      { new: true, runValidators: true }
    );

    if (!professional) {
      throw new AppError('Perfil profesional no encontrado', 404);
    }

    res.json({
      success: true,
      data: professional,
    });
  } catch (error) {
    next(error);
  }
};
