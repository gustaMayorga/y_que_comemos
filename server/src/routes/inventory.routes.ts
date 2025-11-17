import { Router } from 'express';
import {
  getInventory,
  addItem,
  updateItem,
  deleteItem,
  scanReceipt,
  detectFoodFromImage,
} from '../controllers/inventory.controller.js';
import { authenticate } from '../middleware/auth.middleware.js';
import multer from 'multer';

const upload = multer({ dest: 'uploads/' });

const router = Router();

router.use(authenticate);

router.get('/', getInventory);
router.post('/', addItem);
router.put('/:id', updateItem);
router.delete('/:id', deleteItem);
router.post('/scan-receipt', upload.single('image'), scanReceipt);
router.post('/detect-food', upload.single('image'), detectFoodFromImage);

export default router;
