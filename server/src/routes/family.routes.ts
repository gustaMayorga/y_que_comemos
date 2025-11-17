import { Router } from 'express';
import {
  getFamily,
  createFamily,
  updateFamily,
  addMember,
  updateMember,
  deleteMember,
} from '../controllers/family.controller.js';
import { authenticate } from '../middleware/auth.middleware.js';

const router = Router();

router.use(authenticate);

router.get('/', getFamily);
router.post('/', createFamily);
router.put('/', updateFamily);
router.post('/members', addMember);
router.put('/members/:memberId', updateMember);
router.delete('/members/:memberId', deleteMember);

export default router;
