import { Router } from 'express';
import {
  getProfessionals,
  getProfessional,
  createProfessionalProfile,
  updateProfessionalProfile,
} from '../controllers/professional.controller.js';
import { authenticate, authorize } from '../middleware/auth.middleware.js';
import { UserRole } from '../models/User.model.js';

const router = Router();

router.get('/', getProfessionals);
router.get('/:id', getProfessional);

router.use(authenticate);

router.post('/', authorize(UserRole.PROFESSIONAL), createProfessionalProfile);
router.put('/', authorize(UserRole.PROFESSIONAL), updateProfessionalProfile);

export default router;
