import mongoose, { Document, Schema } from 'mongoose';

export enum FoodLocation {
  FRIDGE = 'fridge',
  PANTRY = 'pantry',
  FREEZER = 'freezer'
}

export enum FoodCategory {
  FRUITS = 'fruits',
  VEGETABLES = 'vegetables',
  PROTEINS = 'proteins',
  DAIRY = 'dairy',
  GRAINS = 'grains',
  SNACKS = 'snacks',
  BEVERAGES = 'beverages',
  CONDIMENTS = 'condiments',
  OTHER = 'other'
}

export interface INutritionalInfo {
  calories: number;
  protein: number;
  carbs: number;
  fat: number;
  fiber: number;
}

export interface IFoodItem extends Document {
  userId: mongoose.Types.ObjectId;
  name: string;
  category: FoodCategory;
  quantity: number;
  unit: string;
  location: FoodLocation;
  expirationDate?: Date;
  purchaseDate: Date;
  nutritionalInfo?: INutritionalInfo;
  createdAt: Date;
  updatedAt: Date;
}

const foodItemSchema = new Schema<IFoodItem>(
  {
    userId: {
      type: Schema.Types.ObjectId,
      ref: 'User',
      required: true,
    },
    name: {
      type: String,
      required: true,
      trim: true,
    },
    category: {
      type: String,
      enum: Object.values(FoodCategory),
      required: true,
    },
    quantity: {
      type: Number,
      required: true,
      min: 0,
    },
    unit: {
      type: String,
      required: true,
    },
    location: {
      type: String,
      enum: Object.values(FoodLocation),
      required: true,
    },
    expirationDate: {
      type: Date,
    },
    purchaseDate: {
      type: Date,
      default: Date.now,
    },
    nutritionalInfo: {
      calories: { type: Number },
      protein: { type: Number },
      carbs: { type: Number },
      fat: { type: Number },
      fiber: { type: Number },
    },
  },
  {
    timestamps: true,
  }
);

// Index for efficient queries
foodItemSchema.index({ userId: 1, category: 1 });
foodItemSchema.index({ userId: 1, expirationDate: 1 });

export const FoodItem = mongoose.model<IFoodItem>('FoodItem', foodItemSchema);
