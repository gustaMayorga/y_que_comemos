import mongoose, { Document, Schema } from 'mongoose';

export enum RecipeDifficulty {
  EASY = 'easy',
  MEDIUM = 'medium',
  HARD = 'hard'
}

export interface IRecipeIngredient {
  foodItem: string;
  quantity: number;
  unit: string;
}

export interface INutritionalInfo {
  calories: number;
  protein: number;
  carbs: number;
  fat: number;
  fiber: number;
}

export interface IRecipe extends Document {
  userId: mongoose.Types.ObjectId;
  name: string;
  description: string;
  image?: string;
  difficulty: RecipeDifficulty;
  prepTime: number;
  cookTime: number;
  servings: number;
  ingredients: IRecipeIngredient[];
  instructions: string[];
  nutritionalInfo: INutritionalInfo;
  tags: string[];
  isCustom: boolean;
  createdBy?: mongoose.Types.ObjectId;
  isAIGenerated: boolean;
  createdAt: Date;
  updatedAt: Date;
}

const recipeSchema = new Schema<IRecipe>(
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
    description: {
      type: String,
      required: true,
    },
    image: {
      type: String,
    },
    difficulty: {
      type: String,
      enum: Object.values(RecipeDifficulty),
      required: true,
    },
    prepTime: {
      type: Number,
      required: true,
    },
    cookTime: {
      type: Number,
      required: true,
    },
    servings: {
      type: Number,
      required: true,
      min: 1,
    },
    ingredients: [
      {
        foodItem: { type: String, required: true },
        quantity: { type: Number, required: true },
        unit: { type: String, required: true },
      },
    ],
    instructions: [{ type: String, required: true }],
    nutritionalInfo: {
      calories: { type: Number, required: true },
      protein: { type: Number, required: true },
      carbs: { type: Number, required: true },
      fat: { type: Number, required: true },
      fiber: { type: Number, required: true },
    },
    tags: [{ type: String }],
    isCustom: {
      type: Boolean,
      default: false,
    },
    createdBy: {
      type: Schema.Types.ObjectId,
      ref: 'User',
    },
    isAIGenerated: {
      type: Boolean,
      default: false,
    },
  },
  {
    timestamps: true,
  }
);

// Indexes
recipeSchema.index({ userId: 1, name: 1 });
recipeSchema.index({ tags: 1 });

export const Recipe = mongoose.model<IRecipe>('Recipe', recipeSchema);
