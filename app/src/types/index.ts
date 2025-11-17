// User types
export enum UserRole {
  FAMILY = 'family',
  PROFESSIONAL = 'professional',
  ADMIN = 'admin'
}

export enum SubscriptionTier {
  FREE = 'free',
  PREMIUM = 'premium',
  PROFESSIONAL = 'professional'
}

export interface User {
  id: string;
  email: string;
  name: string;
  role: UserRole;
  subscription: SubscriptionTier;
  avatar?: string;
  createdAt: string;
}

// Family types
export interface FamilyMember {
  id: string;
  name: string;
  age: number;
  weight?: number; // kg
  height?: number; // cm
  gender?: 'male' | 'female' | 'other';
  dietaryRestrictions: string[];
  allergies: string[];
  bmi?: number;
  dailyCalories?: number;
}

export interface Family {
  id: string;
  userId: string;
  members: FamilyMember[];
  mealSchedule: {
    breakfast: string; // "08:00"
    lunch: string;
    dinner: string;
  };
  preferences: string[];
}

// Food & Inventory types
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

export interface FoodItem {
  id: string;
  userId: string;
  name: string;
  category: FoodCategory;
  quantity: number;
  unit: string; // 'kg', 'g', 'L', 'ml', 'units'
  location: FoodLocation;
  expirationDate?: string;
  purchaseDate: string;
  nutritionalInfo?: {
    calories: number; // per 100g/100ml
    protein: number;
    carbs: number;
    fat: number;
    fiber: number;
  };
}

// Recipe types
export enum RecipeDifficulty {
  EASY = 'easy',
  MEDIUM = 'medium',
  HARD = 'hard'
}

export interface RecipeIngredient {
  foodItem: string; // food name
  quantity: number;
  unit: string;
}

export interface Recipe {
  id: string;
  name: string;
  description: string;
  image?: string;
  difficulty: RecipeDifficulty;
  prepTime: number; // minutes
  cookTime: number;
  servings: number;
  ingredients: RecipeIngredient[];
  instructions: string[];
  nutritionalInfo: {
    calories: number;
    protein: number;
    carbs: number;
    fat: number;
    fiber: number;
  };
  tags: string[];
  isCustom: boolean; // user-created vs AI-generated
  createdBy?: string;
}

// Meal Plan types
export enum MealType {
  BREAKFAST = 'breakfast',
  LUNCH = 'lunch',
  DINNER = 'dinner',
  SNACK = 'snack'
}

export interface PlannedMeal {
  id: string;
  date: string; // ISO date
  mealType: MealType;
  recipeId: string;
  recipe: Recipe;
  memberIds: string[]; // which family members
  notes?: string;
}

export interface MealPlan {
  id: string;
  userId: string;
  startDate: string;
  endDate: string;
  meals: PlannedMeal[];
  shoppingList: ShoppingListItem[];
}

export interface ShoppingListItem {
  foodItem: string;
  quantity: number;
  unit: string;
  category: FoodCategory;
  purchased: boolean;
}

// Professional types
export interface Professional {
  id: string;
  userId: string;
  specialization: string[];
  bio: string;
  experience: number; // years
  rating: number;
  reviewCount: number;
  hourlyRate: number;
  availability: {
    day: string;
    slots: string[]; // ["09:00", "10:00", ...]
  }[];
  certifications: string[];
}

export interface Consultation {
  id: string;
  clientId: string;
  professionalId: string;
  date: string;
  duration: number; // minutes
  status: 'pending' | 'confirmed' | 'completed' | 'cancelled';
  notes?: string;
  amount: number;
}

// API Response types
export interface ApiResponse<T> {
  success: boolean;
  data?: T;
  error?: string;
  message?: string;
}

export interface PaginatedResponse<T> {
  data: T[];
  total: number;
  page: number;
  pageSize: number;
  totalPages: number;
}
