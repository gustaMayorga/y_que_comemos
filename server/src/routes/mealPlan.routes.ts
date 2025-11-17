import { Router } from 'express';
import {
  getMealPlans,
  getMealPlan,
  createMealPlan,
  updateMealPlan,
  deleteMealPlan,
  generateWeeklyPlan,
} from '../controllers/mealPlan.controller.js';
import { authenticate } from '../middleware/auth.middleware.js';

const router = Router();

router.use(authenticate);

router.get('/', getMealPlans);
router.get('/:id', getMealPlan);
router.post('/', createMealPlan);
router.put('/:id', updateMealPlan);
router.delete('/:id', deleteMealPlan);
router.post('/generate-weekly', generateWeeklyPlan);

export default router;
