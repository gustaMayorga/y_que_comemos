import { Response, NextFunction } from 'express';
import { MealPlan } from '../models/MealPlan.model.js';
import { AuthRequest } from '../middleware/auth.middleware.js';
import { AppError } from '../middleware/errorHandler.js';

export const getMealPlans = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const mealPlans = await MealPlan.find({ userId: req.user._id })
      .populate('meals.recipeId')
      .sort({ startDate: -1 });

    res.json({
      success: true,
      data: mealPlans,
    });
  } catch (error) {
    next(error);
  }
};

export const getMealPlan = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const mealPlan = await MealPlan.findOne({
      _id: id,
      userId: req.user._id,
    }).populate('meals.recipeId');

    if (!mealPlan) {
      throw new AppError('Plan de comidas no encontrado', 404);
    }

    res.json({
      success: true,
      data: mealPlan,
    });
  } catch (error) {
    next(error);
  }
};

export const createMealPlan = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const mealPlan = await MealPlan.create({
      ...req.body,
      userId: req.user._id,
    });

    res.status(201).json({
      success: true,
      data: mealPlan,
    });
  } catch (error) {
    next(error);
  }
};

export const updateMealPlan = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const mealPlan = await MealPlan.findOneAndUpdate(
      { _id: id, userId: req.user._id },
      req.body,
      { new: true, runValidators: true }
    ).populate('meals.recipeId');

    if (!mealPlan) {
      throw new AppError('Plan de comidas no encontrado', 404);
    }

    res.json({
      success: true,
      data: mealPlan,
    });
  } catch (error) {
    next(error);
  }
};

export const deleteMealPlan = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const mealPlan = await MealPlan.findOneAndDelete({
      _id: id,
      userId: req.user._id,
    });

    if (!mealPlan) {
      throw new AppError('Plan de comidas no encontrado', 404);
    }

    res.json({
      success: true,
      message: 'Plan de comidas eliminado exitosamente',
    });
  } catch (error) {
    next(error);
  }
};

export const generateWeeklyPlan = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    // TODO: Implement AI-based weekly meal plan generation
    // This would analyze family members, dietary restrictions, inventory, and preferences
    // to create an optimal weekly meal plan

    res.json({
      success: true,
      message: 'Funcionalidad de generación automática en desarrollo',
    });
  } catch (error) {
    next(error);
  }
};
