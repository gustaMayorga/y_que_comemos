import mongoose, { Document, Schema } from 'mongoose';

export enum MealType {
  BREAKFAST = 'breakfast',
  LUNCH = 'lunch',
  DINNER = 'dinner',
  SNACK = 'snack'
}

export interface IPlannedMeal {
  date: Date;
  mealType: MealType;
  recipeId: mongoose.Types.ObjectId;
  memberIds: string[];
  notes?: string;
}

export interface IShoppingListItem {
  foodItem: string;
  quantity: number;
  unit: string;
  category: string;
  purchased: boolean;
}

export interface IMealPlan extends Document {
  userId: mongoose.Types.ObjectId;
  startDate: Date;
  endDate: Date;
  meals: IPlannedMeal[];
  shoppingList: IShoppingListItem[];
  createdAt: Date;
  updatedAt: Date;
}

const mealPlanSchema = new Schema<IMealPlan>(
  {
    userId: {
      type: Schema.Types.ObjectId,
      ref: 'User',
      required: true,
    },
    startDate: {
      type: Date,
      required: true,
    },
    endDate: {
      type: Date,
      required: true,
    },
    meals: [
      {
        date: { type: Date, required: true },
        mealType: {
          type: String,
          enum: Object.values(MealType),
          required: true,
        },
        recipeId: {
          type: Schema.Types.ObjectId,
          ref: 'Recipe',
          required: true,
        },
        memberIds: [{ type: String }],
        notes: { type: String },
      },
    ],
    shoppingList: [
      {
        foodItem: { type: String, required: true },
        quantity: { type: Number, required: true },
        unit: { type: String, required: true },
        category: { type: String, required: true },
        purchased: { type: Boolean, default: false },
      },
    ],
  },
  {
    timestamps: true,
  }
);

// Indexes
mealPlanSchema.index({ userId: 1, startDate: 1, endDate: 1 });

export const MealPlan = mongoose.model<IMealPlan>('MealPlan', mealPlanSchema);
