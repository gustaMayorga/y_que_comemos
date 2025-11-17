import { useState } from 'react';
import { Search, Star, Calendar, Video, MessageCircle, Stethoscope } from 'lucide-react';
import { Professional } from '../types';

export default function NutritionistsPage() {
  const [professionals, setProfessionals] = useState<Professional[]>([]);
  const [searchTerm, setSearchTerm] = useState('');
  const [selectedSpecialization, setSelectedSpecialization] = useState('all');

  const filteredProfessionals = professionals.filter((prof) => {
    const matchesSearch = searchTerm === '' ||
      prof.bio.toLowerCase().includes(searchTerm.toLowerCase());
    const matchesSpec = selectedSpecialization === 'all' ||
      prof.specialization.includes(selectedSpecialization);
    return matchesSearch && matchesSpec;
  });

  return (
    <div className="space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold text-gray-900">Nutricionistas</h1>
        <p className="text-gray-600 mt-1">
          Conecta con profesionales certificados para asesoría personalizada
        </p>
      </div>

      {/* Search and Filters */}
      <div className="flex flex-col md:flex-row gap-4">
        <div className="relative flex-1">
          <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-5 w-5 text-gray-400" />
          <input
            type="text"
            placeholder="Buscar por especialidad o nombre..."
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            className="input pl-10"
          />
        </div>

        <select
          value={selectedSpecialization}
          onChange={(e) => setSelectedSpecialization(e.target.value)}
          className="input md:w-64"
        >
          <option value="all">Todas las especialidades</option>
          <option value="weight-loss">Pérdida de peso</option>
          <option value="sports">Nutrición deportiva</option>
          <option value="diabetes">Diabetes</option>
          <option value="pediatric">Pediátrica</option>
          <option value="vegan">Vegana/Vegetariana</option>
        </select>
      </div>

      {/* Featured Banner */}
      <div className="card bg-gradient-to-r from-primary-600 to-primary-700 text-white">
        <div className="flex items-start space-x-4">
          <div className="h-12 w-12 bg-white bg-opacity-20 rounded-lg flex items-center justify-center flex-shrink-0">
            <Stethoscope className="h-6 w-6" />
          </div>
          <div className="flex-1">
            <h3 className="font-semibold text-lg mb-1">Primera Consulta Gratis</h3>
            <p className="text-primary-100 text-sm mb-3">
              Conoce a nuestros nutricionistas con una sesión de 30 minutos sin costo
            </p>
            <button className="btn bg-white text-primary-600 hover:bg-gray-100 text-sm">
              Ver Profesionales
            </button>
          </div>
        </div>
      </div>

      {/* Professionals Grid */}
      {filteredProfessionals.length === 0 ? (
        <div className="card text-center py-12">
          <Stethoscope className="h-16 w-16 text-gray-300 mx-auto mb-4" />
          <h3 className="text-lg font-semibold text-gray-900 mb-2">
            No hay profesionales disponibles
          </h3>
          <p className="text-gray-600">Intenta con otros filtros de búsqueda</p>
        </div>
      ) : (
        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6">
          {filteredProfessionals.map((prof) => (
            <ProfessionalCard key={prof.id} professional={prof} />
          ))}
        </div>
      )}

      {/* Demo cards for empty state */}
      {professionals.length === 0 && (
        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6">
          <DemoProfessionalCard
            name="Dra. María González"
            specialization={['Pérdida de peso', 'Diabetes']}
            experience={8}
            rating={4.9}
            reviewCount={127}
            hourlyRate={45}
          />
          <DemoProfessionalCard
            name="Dr. Carlos Rodríguez"
            specialization={['Nutrición deportiva', 'Vegana']}
            experience={5}
            rating={4.8}
            reviewCount={89}
            hourlyRate={40}
          />
          <DemoProfessionalCard
            name="Lic. Ana Martínez"
            specialization={['Pediátrica', 'Familiar']}
            experience={12}
            rating={5.0}
            reviewCount={203}
            hourlyRate={50}
          />
        </div>
      )}
    </div>
  );
}

function ProfessionalCard({ professional }: { professional: Professional }) {
  return (
    <div className="card hover:shadow-lg transition-shadow">
      <div className="flex items-start space-x-3 mb-4">
        <div className="h-16 w-16 rounded-full bg-primary-100 flex items-center justify-center flex-shrink-0">
          <Stethoscope className="h-8 w-8 text-primary-600" />
        </div>
        <div className="flex-1">
          <h3 className="font-semibold text-lg">Profesional</h3>
          <div className="flex items-center space-x-1 mt-1">
            <Star className="h-4 w-4 text-yellow-400 fill-current" />
            <span className="text-sm font-medium">{professional.rating}</span>
            <span className="text-sm text-gray-500">({professional.reviewCount})</span>
          </div>
        </div>
      </div>

      <p className="text-sm text-gray-600 mb-4 line-clamp-3">{professional.bio}</p>

      <div className="space-y-2 mb-4">
        <div className="text-xs text-gray-600">Especialidades</div>
        <div className="flex flex-wrap gap-1">
          {professional.specialization.map((spec, index) => (
            <span
              key={index}
              className="px-2 py-1 bg-primary-100 text-primary-700 text-xs rounded-full"
            >
              {spec}
            </span>
          ))}
        </div>
      </div>

      <div className="text-sm text-gray-600 mb-4">
        {professional.experience} años de experiencia
      </div>

      <div className="flex items-center justify-between pt-4 border-t">
        <div>
          <div className="text-2xl font-bold text-primary-600">
            ${professional.hourlyRate}
          </div>
          <div className="text-xs text-gray-500">por hora</div>
        </div>
        <button className="btn-primary text-sm flex items-center space-x-1">
          <Calendar className="h-4 w-4" />
          <span>Agendar</span>
        </button>
      </div>
    </div>
  );
}

function DemoProfessionalCard({
  name,
  specialization,
  experience,
  rating,
  reviewCount,
  hourlyRate,
}: {
  name: string;
  specialization: string[];
  experience: number;
  rating: number;
  reviewCount: number;
  hourlyRate: number;
}) {
  return (
    <div className="card hover:shadow-lg transition-shadow">
      <div className="flex items-start space-x-3 mb-4">
        <div className="h-16 w-16 rounded-full bg-primary-100 flex items-center justify-center flex-shrink-0">
          <Stethoscope className="h-8 w-8 text-primary-600" />
        </div>
        <div className="flex-1">
          <h3 className="font-semibold text-lg">{name}</h3>
          <div className="flex items-center space-x-1 mt-1">
            <Star className="h-4 w-4 text-yellow-400 fill-current" />
            <span className="text-sm font-medium">{rating}</span>
            <span className="text-sm text-gray-500">({reviewCount})</span>
          </div>
        </div>
      </div>

      <p className="text-sm text-gray-600 mb-4">
        Nutricionista certificado con experiencia en tratamientos personalizados
        y seguimiento continuo de pacientes.
      </p>

      <div className="space-y-2 mb-4">
        <div className="text-xs text-gray-600">Especialidades</div>
        <div className="flex flex-wrap gap-1">
          {specialization.map((spec, index) => (
            <span
              key={index}
              className="px-2 py-1 bg-primary-100 text-primary-700 text-xs rounded-full"
            >
              {spec}
            </span>
          ))}
        </div>
      </div>

      <div className="text-sm text-gray-600 mb-4">{experience} años de experiencia</div>

      <div className="flex items-center justify-between pt-4 border-t">
        <div>
          <div className="text-2xl font-bold text-primary-600">${hourlyRate}</div>
          <div className="text-xs text-gray-500">por hora</div>
        </div>
        <button className="btn-primary text-sm flex items-center space-x-1">
          <Calendar className="h-4 w-4" />
          <span>Agendar</span>
        </button>
      </div>
    </div>
  );
}
