import { useState } from 'react';
import { Plus, Users, Trash2, Edit, TrendingUp } from 'lucide-react';
import { FamilyMember } from '../types';
import { toast } from 'sonner';

export default function FamilyPage() {
  const [members, setMembers] = useState<FamilyMember[]>([]);
  const [showAddModal, setShowAddModal] = useState(false);

  const handleAddMember = (member: Partial<FamilyMember>) => {
    // TODO: API call
    toast.success('Miembro agregado a la familia');
    setShowAddModal(false);
  };

  const calculateBMI = (weight: number, height: number) => {
    const heightInMeters = height / 100;
    return (weight / (heightInMeters * heightInMeters)).toFixed(1);
  };

  const calculateDailyCalories = (member: FamilyMember) => {
    // Harris-Benedict equation (simplified)
    if (!member.weight || !member.height) return 2000;

    let bmr;
    if (member.gender === 'male') {
      bmr = 88.362 + 13.397 * member.weight + 4.799 * member.height - 5.677 * member.age;
    } else {
      bmr = 447.593 + 9.247 * member.weight + 3.098 * member.height - 4.330 * member.age;
    }

    // Activity factor (assuming moderate activity)
    return Math.round(bmr * 1.55);
  };

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="flex flex-col md:flex-row md:items-center md:justify-between gap-4">
        <div>
          <h1 className="text-3xl font-bold text-gray-900">Familia</h1>
          <p className="text-gray-600 mt-1">Gestiona los perfiles de tu familia</p>
        </div>

        <button
          onClick={() => setShowAddModal(true)}
          className="btn-primary flex items-center space-x-2"
        >
          <Plus className="h-4 w-4" />
          <span>Agregar Miembro</span>
        </button>
      </div>

      {/* Members Grid */}
      {members.length === 0 ? (
        <div className="card text-center py-12">
          <Users className="h-16 w-16 text-gray-300 mx-auto mb-4" />
          <h3 className="text-lg font-semibold text-gray-900 mb-2">
            Agrega a tu familia
          </h3>
          <p className="text-gray-600 mb-6">
            Crea perfiles para cada miembro y obtén recomendaciones nutricionales personalizadas
          </p>
          <button
            onClick={() => setShowAddModal(true)}
            className="btn-primary"
          >
            Agregar Primer Miembro
          </button>
        </div>
      ) : (
        <div className="grid md:grid-cols-2 gap-6">
          {members.map((member) => (
            <MemberCard
              key={member.id}
              member={member}
              dailyCalories={calculateDailyCalories(member)}
            />
          ))}
        </div>
      )}

      {/* Add Member Modal */}
      {showAddModal && (
        <AddMemberModal onClose={() => setShowAddModal(false)} onAdd={handleAddMember} />
      )}
    </div>
  );
}

function MemberCard({ member, dailyCalories }: { member: FamilyMember; dailyCalories: number }) {
  const bmiStatus = member.bmi
    ? member.bmi < 18.5
      ? 'Bajo peso'
      : member.bmi < 25
      ? 'Normal'
      : member.bmi < 30
      ? 'Sobrepeso'
      : 'Obesidad'
    : 'N/A';

  const bmiColor = member.bmi
    ? member.bmi < 18.5 || member.bmi >= 30
      ? 'text-red-600'
      : member.bmi >= 25
      ? 'text-yellow-600'
      : 'text-green-600'
    : 'text-gray-600';

  return (
    <div className="card">
      <div className="flex items-start justify-between mb-4">
        <div className="flex items-center space-x-3">
          <div className="h-12 w-12 rounded-full bg-primary-100 flex items-center justify-center">
            <Users className="h-6 w-6 text-primary-600" />
          </div>
          <div>
            <h3 className="font-semibold text-lg">{member.name}</h3>
            <p className="text-sm text-gray-600">
              {member.age} años • {member.gender === 'male' ? 'Masculino' : 'Femenino'}
            </p>
          </div>
        </div>
        <div className="flex space-x-1">
          <button className="p-2 text-gray-600 hover:text-primary-600">
            <Edit className="h-4 w-4" />
          </button>
          <button className="p-2 text-gray-600 hover:text-red-600">
            <Trash2 className="h-4 w-4" />
          </button>
        </div>
      </div>

      {member.weight && member.height && (
        <div className="grid grid-cols-3 gap-4 mb-4 p-4 bg-gray-50 rounded-lg">
          <div>
            <div className="text-xs text-gray-600">Peso</div>
            <div className="text-lg font-semibold">{member.weight} kg</div>
          </div>
          <div>
            <div className="text-xs text-gray-600">Altura</div>
            <div className="text-lg font-semibold">{member.height} cm</div>
          </div>
          <div>
            <div className="text-xs text-gray-600">IMC</div>
            <div className={`text-lg font-semibold ${bmiColor}`}>
              {member.bmi}
            </div>
          </div>
        </div>
      )}

      <div className="space-y-3">
        <div className="flex items-center justify-between text-sm">
          <span className="text-gray-600">Estado IMC</span>
          <span className={`font-medium ${bmiColor}`}>{bmiStatus}</span>
        </div>

        <div className="flex items-center justify-between text-sm">
          <span className="text-gray-600">Calorías diarias</span>
          <span className="font-medium">{dailyCalories} kcal</span>
        </div>

        {member.dietaryRestrictions.length > 0 && (
          <div className="pt-3 border-t">
            <div className="text-xs text-gray-600 mb-2">Restricciones dietéticas</div>
            <div className="flex flex-wrap gap-1">
              {member.dietaryRestrictions.map((restriction, index) => (
                <span
                  key={index}
                  className="px-2 py-1 bg-blue-100 text-blue-700 text-xs rounded-full"
                >
                  {restriction}
                </span>
              ))}
            </div>
          </div>
        )}

        {member.allergies.length > 0 && (
          <div className="pt-3 border-t">
            <div className="text-xs text-gray-600 mb-2">Alergias</div>
            <div className="flex flex-wrap gap-1">
              {member.allergies.map((allergy, index) => (
                <span
                  key={index}
                  className="px-2 py-1 bg-red-100 text-red-700 text-xs rounded-full"
                >
                  {allergy}
                </span>
              ))}
            </div>
          </div>
        )}
      </div>
    </div>
  );
}

function AddMemberModal({
  onClose,
  onAdd,
}: {
  onClose: () => void;
  onAdd: (member: Partial<FamilyMember>) => void;
}) {
  const [formData, setFormData] = useState({
    name: '',
    age: 30,
    weight: 70,
    height: 170,
    gender: 'male' as 'male' | 'female' | 'other',
    dietaryRestrictions: [] as string[],
    allergies: [] as string[],
  });

  const bmi = formData.weight && formData.height
    ? (formData.weight / Math.pow(formData.height / 100, 2)).toFixed(1)
    : null;

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    onAdd({
      ...formData,
      bmi: bmi ? Number(bmi) : undefined,
    });
  };

  return (
    <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 p-4">
      <div className="bg-white rounded-lg max-w-2xl w-full p-6 max-h-[90vh] overflow-y-auto">
        <h2 className="text-xl font-bold mb-6">Agregar Miembro de Familia</h2>

        <form onSubmit={handleSubmit} className="space-y-4">
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">
              Nombre completo
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
              <label className="block text-sm font-medium text-gray-700 mb-1">Edad</label>
              <input
                type="number"
                value={formData.age}
                onChange={(e) => setFormData({ ...formData, age: Number(e.target.value) })}
                className="input"
                min="1"
                required
              />
            </div>

            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Género</label>
              <select
                value={formData.gender}
                onChange={(e) =>
                  setFormData({ ...formData, gender: e.target.value as any })
                }
                className="input"
              >
                <option value="male">Masculino</option>
                <option value="female">Femenino</option>
                <option value="other">Otro</option>
              </select>
            </div>
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">
                Peso (kg)
              </label>
              <input
                type="number"
                value={formData.weight}
                onChange={(e) => setFormData({ ...formData, weight: Number(e.target.value) })}
                className="input"
                step="0.1"
                min="1"
              />
            </div>

            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">
                Altura (cm)
              </label>
              <input
                type="number"
                value={formData.height}
                onChange={(e) => setFormData({ ...formData, height: Number(e.target.value) })}
                className="input"
                min="1"
              />
            </div>
          </div>

          {bmi && (
            <div className="p-4 bg-primary-50 rounded-lg">
              <div className="flex items-center space-x-2">
                <TrendingUp className="h-5 w-5 text-primary-600" />
                <span className="text-sm font-medium">IMC: {bmi}</span>
              </div>
            </div>
          )}

          <div className="flex space-x-3 pt-4 border-t">
            <button type="button" onClick={onClose} className="btn-secondary flex-1">
              Cancelar
            </button>
            <button type="submit" className="btn-primary flex-1">
              Agregar Miembro
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
