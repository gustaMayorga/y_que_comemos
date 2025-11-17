import { useState, useRef } from 'react';
import {
  Plus,
  Camera,
  Scan,
  Search,
  Package,
  Trash2,
  Edit,
  Calendar
} from 'lucide-react';
import { toast } from 'sonner';
import { FoodItem, FoodCategory, FoodLocation } from '../types';

export default function InventoryPage() {
  const [items, setItems] = useState<FoodItem[]>([]);
  const [showAddModal, setShowAddModal] = useState(false);
  const [addMethod, setAddMethod] = useState<'manual' | 'scan' | 'photo'>('manual');
  const [searchTerm, setSearchTerm] = useState('');
  const fileInputRef = useRef<HTMLInputElement>(null);

  const handleAddItem = (item: Partial<FoodItem>) => {
    // TODO: API call to add item
    toast.success('Alimento agregado al inventario');
    setShowAddModal(false);
  };

  const handlePhotoUpload = async (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;

    // TODO: Send to AI vision API
    toast.loading('Analizando imagen...');

    // Simulated API call
    setTimeout(() => {
      toast.success('¡3 alimentos detectados!');
      // Add detected items to inventory
    }, 2000);
  };

  const handleScanReceipt = () => {
    // TODO: Implement OCR for receipt scanning
    toast.info('Funcionalidad de escaneo próximamente');
  };

  const filteredItems = items.filter(item =>
    item.name.toLowerCase().includes(searchTerm.toLowerCase())
  );

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="flex flex-col md:flex-row md:items-center md:justify-between gap-4">
        <div>
          <h1 className="text-3xl font-bold text-gray-900">Inventario</h1>
          <p className="text-gray-600 mt-1">Gestiona los alimentos disponibles en tu hogar</p>
        </div>

        <div className="flex gap-2">
          <button
            onClick={() => {
              setAddMethod('photo');
              fileInputRef.current?.click();
            }}
            className="btn-outline flex items-center space-x-2"
          >
            <Camera className="h-4 w-4" />
            <span>Foto</span>
          </button>
          <button
            onClick={handleScanReceipt}
            className="btn-outline flex items-center space-x-2"
          >
            <Scan className="h-4 w-4" />
            <span>Escanear</span>
          </button>
          <button
            onClick={() => {
              setAddMethod('manual');
              setShowAddModal(true);
            }}
            className="btn-primary flex items-center space-x-2"
          >
            <Plus className="h-4 w-4" />
            <span>Agregar</span>
          </button>
        </div>

        <input
          ref={fileInputRef}
          type="file"
          accept="image/*"
          capture="environment"
          onChange={handlePhotoUpload}
          className="hidden"
        />
      </div>

      {/* Search */}
      <div className="relative">
        <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-5 w-5 text-gray-400" />
        <input
          type="text"
          placeholder="Buscar alimentos..."
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
          className="input pl-10"
        />
      </div>

      {/* Inventory Stats */}
      <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
        <div className="card">
          <div className="text-sm text-gray-600">Heladera</div>
          <div className="text-2xl font-bold mt-1">24</div>
        </div>
        <div className="card">
          <div className="text-sm text-gray-600">Alacena</div>
          <div className="text-2xl font-bold mt-1">18</div>
        </div>
        <div className="card">
          <div className="text-sm text-gray-600">Freezer</div>
          <div className="text-2xl font-bold mt-1">8</div>
        </div>
        <div className="card">
          <div className="text-sm text-gray-600">Por vencer</div>
          <div className="text-2xl font-bold mt-1 text-red-600">3</div>
        </div>
      </div>

      {/* Inventory List */}
      {filteredItems.length === 0 ? (
        <div className="card text-center py-12">
          <Package className="h-16 w-16 text-gray-300 mx-auto mb-4" />
          <h3 className="text-lg font-semibold text-gray-900 mb-2">
            Tu inventario está vacío
          </h3>
          <p className="text-gray-600 mb-6">
            Comienza agregando alimentos con foto, escaneo o manualmente
          </p>
          <button
            onClick={() => {
              setAddMethod('manual');
              setShowAddModal(true);
            }}
            className="btn-primary"
          >
            Agregar primer alimento
          </button>
        </div>
      ) : (
        <div className="card">
          <div className="space-y-4">
            {filteredItems.map((item) => (
              <InventoryItem key={item.id} item={item} />
            ))}
          </div>
        </div>
      )}

      {/* Add Item Modal */}
      {showAddModal && (
        <AddItemModal
          onClose={() => setShowAddModal(false)}
          onAdd={handleAddItem}
        />
      )}
    </div>
  );
}

function InventoryItem({ item }: { item: FoodItem }) {
  const daysUntilExpiration = item.expirationDate
    ? Math.ceil((new Date(item.expirationDate).getTime() - Date.now()) / (1000 * 60 * 60 * 24))
    : null;

  return (
    <div className="flex items-center justify-between p-4 rounded-lg border border-gray-200 hover:border-gray-300">
      <div className="flex items-center space-x-4 flex-1">
        <div className="h-12 w-12 rounded-lg bg-primary-100 flex items-center justify-center">
          <Package className="h-6 w-6 text-primary-600" />
        </div>
        <div className="flex-1">
          <div className="font-medium">{item.name}</div>
          <div className="text-sm text-gray-500">
            {item.quantity} {item.unit} • {item.location}
          </div>
        </div>
        {daysUntilExpiration !== null && (
          <div className={`text-sm ${daysUntilExpiration <= 3 ? 'text-red-600' : 'text-gray-600'}`}>
            <Calendar className="h-4 w-4 inline mr-1" />
            {daysUntilExpiration > 0 ? `${daysUntilExpiration} días` : 'Vencido'}
          </div>
        )}
      </div>
      <div className="flex items-center space-x-2 ml-4">
        <button className="p-2 text-gray-600 hover:text-primary-600">
          <Edit className="h-4 w-4" />
        </button>
        <button className="p-2 text-gray-600 hover:text-red-600">
          <Trash2 className="h-4 w-4" />
        </button>
      </div>
    </div>
  );
}

function AddItemModal({
  onClose,
  onAdd,
}: {
  onClose: () => void;
  onAdd: (item: Partial<FoodItem>) => void;
}) {
  const [formData, setFormData] = useState({
    name: '',
    category: FoodCategory.OTHER,
    quantity: 1,
    unit: 'units',
    location: FoodLocation.PANTRY,
    expirationDate: '',
  });

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    onAdd(formData);
  };

  return (
    <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 p-4">
      <div className="bg-white rounded-lg max-w-md w-full p-6">
        <h2 className="text-xl font-bold mb-4">Agregar Alimento</h2>

        <form onSubmit={handleSubmit} className="space-y-4">
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">
              Nombre del alimento
            </label>
            <input
              type="text"
              value={formData.name}
              onChange={(e) => setFormData({ ...formData, name: e.target.value })}
              className="input"
              required
            />
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">
                Cantidad
              </label>
              <input
                type="number"
                value={formData.quantity}
                onChange={(e) => setFormData({ ...formData, quantity: Number(e.target.value) })}
                className="input"
                min="0"
                step="0.1"
                required
              />
            </div>

            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">
                Unidad
              </label>
              <select
                value={formData.unit}
                onChange={(e) => setFormData({ ...formData, unit: e.target.value })}
                className="input"
              >
                <option value="units">Unidades</option>
                <option value="kg">Kilogramos</option>
                <option value="g">Gramos</option>
                <option value="L">Litros</option>
                <option value="ml">Mililitros</option>
              </select>
            </div>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">
              Categoría
            </label>
            <select
              value={formData.category}
              onChange={(e) => setFormData({ ...formData, category: e.target.value as FoodCategory })}
              className="input"
            >
              <option value={FoodCategory.FRUITS}>Frutas</option>
              <option value={FoodCategory.VEGETABLES}>Verduras</option>
              <option value={FoodCategory.PROTEINS}>Proteínas</option>
              <option value={FoodCategory.DAIRY}>Lácteos</option>
              <option value={FoodCategory.GRAINS}>Granos</option>
              <option value={FoodCategory.OTHER}>Otro</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">
              Ubicación
            </label>
            <select
              value={formData.location}
              onChange={(e) => setFormData({ ...formData, location: e.target.value as FoodLocation })}
              className="input"
            >
              <option value={FoodLocation.FRIDGE}>Heladera</option>
              <option value={FoodLocation.PANTRY}>Alacena</option>
              <option value={FoodLocation.FREEZER}>Freezer</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">
              Fecha de vencimiento (opcional)
            </label>
            <input
              type="date"
              value={formData.expirationDate}
              onChange={(e) => setFormData({ ...formData, expirationDate: e.target.value })}
              className="input"
            />
          </div>

          <div className="flex space-x-3 pt-4">
            <button
              type="button"
              onClick={onClose}
              className="btn-secondary flex-1"
            >
              Cancelar
            </button>
            <button
              type="submit"
              className="btn-primary flex-1"
            >
              Agregar
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
