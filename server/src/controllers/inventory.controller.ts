import { Response, NextFunction } from 'express';
import { FoodItem } from '../models/FoodItem.model.js';
import { AuthRequest } from '../middleware/auth.middleware.js';
import { AppError } from '../middleware/errorHandler.js';
import { detectFoodItems } from '../services/ai.service.js';
import { extractTextFromImage } from '../services/ocr.service.js';

export const getInventory = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { category, location, search } = req.query;

    const filter: any = { userId: req.user._id };

    if (category) filter.category = category;
    if (location) filter.location = location;
    if (search) {
      filter.name = { $regex: search, $options: 'i' };
    }

    const items = await FoodItem.find(filter).sort({ createdAt: -1 });

    res.json({
      success: true,
      data: items,
    });
  } catch (error) {
    next(error);
  }
};

export const addItem = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const itemData = {
      ...req.body,
      userId: req.user._id,
    };

    const item = await FoodItem.create(itemData);

    res.status(201).json({
      success: true,
      data: item,
    });
  } catch (error) {
    next(error);
  }
};

export const updateItem = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const item = await FoodItem.findOneAndUpdate(
      { _id: id, userId: req.user._id },
      req.body,
      { new: true, runValidators: true }
    );

    if (!item) {
      throw new AppError('Item no encontrado', 404);
    }

    res.json({
      success: true,
      data: item,
    });
  } catch (error) {
    next(error);
  }
};

export const deleteItem = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    const { id } = req.params;

    const item = await FoodItem.findOneAndDelete({
      _id: id,
      userId: req.user._id,
    });

    if (!item) {
      throw new AppError('Item no encontrado', 404);
    }

    res.json({
      success: true,
      message: 'Item eliminado exitosamente',
    });
  } catch (error) {
    next(error);
  }
};

export const scanReceipt = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    if (!req.file) {
      throw new AppError('Por favor proporciona una imagen', 400);
    }

    // Extract text from receipt using OCR
    const text = await extractTextFromImage(req.file.path);

    // TODO: Parse receipt text to extract items
    // This would require more sophisticated parsing logic

    res.json({
      success: true,
      data: {
        text,
        message: 'Ticket procesado. Implementa la lógica de parsing para extraer items.',
      },
    });
  } catch (error) {
    next(error);
  }
};

export const detectFoodFromImage = async (
  req: AuthRequest,
  res: Response,
  next: NextFunction
) => {
  try {
    if (!req.file) {
      throw new AppError('Por favor proporciona una imagen', 400);
    }

    // Use AI to detect food items in image
    const detectedItems = await detectFoodItems(req.file.path);

    // Add detected items to inventory
    const items = await Promise.all(
      detectedItems.map((item) =>
        FoodItem.create({
          ...item,
          userId: req.user._id,
        })
      )
    );

    res.json({
      success: true,
      data: items,
      message: `${items.length} alimentos detectados y agregados al inventario`,
    });
  } catch (error) {
    next(error);
  }
};
