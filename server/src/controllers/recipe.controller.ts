import { Response, NextFunction } from 'express';
import { Recipe } from '../models/Recipe.model.js';
import { FoodItem } from '../models/FoodItem.model.js';
import { Family } from '../models/Family.model.js';
import { AuthRequest } from '../middleware/auth.middleware.js';
import { AppError } from '../middleware/errorHandler.js';
import { generateRecipesWithAI } from '../services/ai.service.js';

export const getRecipes = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { difficulty, search, tags } = req.query;

    const filter: any = { userId: req.user._id };

    if (difficulty) filter.difficulty = difficulty;
    if (tags) filter.tags = { $in: (tags as string).split(',') };
    if (search) {
      filter.name = { $regex: search, $options: 'i' };
    }

    const recipes = await Recipe.find(filter).sort({ createdAt: -1 });

    res.json({
      success: true,
      data: recipes,
    });
  } catch (error) {
    next(error);
  }
};

export const getRecipe = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const recipe = await Recipe.findOne({ _id: id, userId: req.user._id });

    if (!recipe) {
      throw new AppError('Receta no encontrada', 404);
    }

    res.json({
      success: true,
      data: recipe,
    });
  } catch (error) {
    next(error);
  }
};

export const createRecipe = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const recipeData = {
      ...req.body,
      userId: req.user._id,
      isCustom: true,
    };

    const recipe = await Recipe.create(recipeData);

    res.status(201).json({
      success: true,
      data: recipe,
    });
  } catch (error) {
    next(error);
  }
};

export const updateRecipe = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const recipe = await Recipe.findOneAndUpdate(
      { _id: id, userId: req.user._id },
      req.body,
      { new: true, runValidators: true }
    );

    if (!recipe) {
      throw new AppError('Receta no encontrada', 404);
    }

    res.json({
      success: true,
      data: recipe,
    });
  } catch (error) {
    next(error);
  }
};

export const deleteRecipe = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const recipe = await Recipe.findOneAndDelete({
      _id: id,
      userId: req.user._id,
    });

    if (!recipe) {
      throw new AppError('Receta no encontrada', 404);
    }

    res.json({
      success: true,
      message: 'Receta eliminada exitosamente',
    });
  } catch (error) {
    next(error);
  }
};

export const generateRecipes = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const {
      servings = 4,
      difficulty,
      mealType,
      dietaryRestrictions = [],
      maxTime = 60,
      useInventory = true,
    } = req.body;

    let availableIngredients: string[] = [];

    if (useInventory) {
      // Get user's inventory
      const inventory = await FoodItem.find({ userId: req.user._id });
      availableIngredients = inventory.map((item) => item.name);
    }

    // Get family dietary restrictions
    const family = await Family.findOne({ userId: req.user._id });
    const allDietaryRestrictions = [
      ...dietaryRestrictions,
      ...(family?.members.flatMap((m) => m.dietaryRestrictions) || []),
    ];
    const allAllergies = family?.members.flatMap((m) => m.allergies) || [];

    // Generate recipes using AI
    const generatedRecipes = await generateRecipesWithAI({
      availableIngredients,
      servings,
      difficulty,
      mealType,
      dietaryRestrictions: [...new Set(allDietaryRestrictions)],
      allergies: [...new Set(allAllergies)],
      maxTime,
    });

    // Save generated recipes
    const recipes = await Promise.all(
      generatedRecipes.map((recipe) =>
        Recipe.create({
          ...recipe,
          userId: req.user._id,
          isAIGenerated: true,
        })
      )
    );

    res.json({
      success: true,
      data: recipes,
      message: `${recipes.length} recetas generadas exitosamente`,
    });
  } catch (error) {
    next(error);
  }
};
