import mongoose, { Document, Schema } from 'mongoose';

export interface IAvailabilitySlot {
  day: string;
  slots: string[];
}

export interface IProfessional extends Document {
  userId: mongoose.Types.ObjectId;
  specialization: string[];
  bio: string;
  experience: number;
  rating: number;
  reviewCount: number;
  hourlyRate: number;
  availability: IAvailabilitySlot[];
  certifications: string[];
  isVerified: boolean;
  createdAt: Date;
  updatedAt: Date;
}

const professionalSchema = new Schema<IProfessional>(
  {
    userId: {
      type: Schema.Types.ObjectId,
      ref: 'User',
      required: true,
      unique: true,
    },
    specialization: [{ type: String, required: true }],
    bio: {
      type: String,
      required: true,
    },
    experience: {
      type: Number,
      required: true,
      min: 0,
    },
    rating: {
      type: Number,
      default: 0,
      min: 0,
      max: 5,
    },
    reviewCount: {
      type: Number,
      default: 0,
    },
    hourlyRate: {
      type: Number,
      required: true,
      min: 0,
    },
    availability: [
      {
        day: { type: String, required: true },
        slots: [{ type: String }],
      },
    ],
    certifications: [{ type: String }],
    isVerified: {
      type: Boolean,
      default: false,
    },
  },
  {
    timestamps: true,
  }
);

// Indexes
professionalSchema.index({ specialization: 1 });
professionalSchema.index({ rating: -1 });

export const Professional = mongoose.model<IProfessional>('Professional', professionalSchema);
