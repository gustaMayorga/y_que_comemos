import mongoose, { Document, Schema } from 'mongoose';

export interface IFamilyMember {
  name: string;
  age: number;
  weight?: number;
  height?: number;
  gender?: 'male' | 'female' | 'other';
  dietaryRestrictions: string[];
  allergies: string[];
  bmi?: number;
  dailyCalories?: number;
}

export interface IFamily extends Document {
  userId: mongoose.Types.ObjectId;
  members: IFamilyMember[];
  mealSchedule: {
    breakfast: string;
    lunch: string;
    dinner: string;
  };
  preferences: string[];
  createdAt: Date;
  updatedAt: Date;
}

const familyMemberSchema = new Schema<IFamilyMember>({
  name: { type: String, required: true },
  age: { type: Number, required: true },
  weight: { type: Number },
  height: { type: Number },
  gender: { type: String, enum: ['male', 'female', 'other'] },
  dietaryRestrictions: [{ type: String }],
  allergies: [{ type: String }],
  bmi: { type: Number },
  dailyCalories: { type: Number },
});

const familySchema = new Schema<IFamily>(
  {
    userId: {
      type: Schema.Types.ObjectId,
      ref: 'User',
      required: true,
      unique: true,
    },
    members: [familyMemberSchema],
    mealSchedule: {
      breakfast: { type: String, default: '08:00' },
      lunch: { type: String, default: '13:00' },
      dinner: { type: String, default: '20:00' },
    },
    preferences: [{ type: String }],
  },
  {
    timestamps: true,
  }
);

export const Family = mongoose.model<IFamily>('Family', familySchema);
