#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Error/ErrorWarningAssert.hpp"
#include "Engine/EngineCommon.hpp"


STATIC const Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4(
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
);


Matrix4x4::Matrix4x4( float in0, float in1, float in2, float in3, float in4, float in5, float in6, float in7, 
					  float in8, float in9, float in10, float in11, float in12, float in13, float in14, float in15, Ordering ordering /*= ROW_MAJOR*/ )
{
	if ( ordering == ROW_MAJOR )
	{
		m_data[ 0 ] = in0;		m_data[ 1 ] = in1;		m_data[ 2 ] = in2;		m_data[ 3 ] = in3;
		m_data[ 4 ] = in4;		m_data[ 5 ] = in5;		m_data[ 6 ] = in6;		m_data[ 7 ] = in7;
		m_data[ 8 ] = in8;		m_data[ 9 ] = in9;		m_data[ 10 ] = in10;	m_data[ 11 ] = in11;
		m_data[ 12 ] = in12;	m_data[ 13 ] = in13;	m_data[ 14 ] = in14;	m_data[ 15 ] = in15;
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		m_data[ 0 ] = in0;		m_data[ 1 ] = in4;		m_data[ 2 ] = in8;		m_data[ 3 ] = in12;
		m_data[ 4 ] = in1;		m_data[ 5 ] = in5;		m_data[ 6 ] = in9;		m_data[ 7 ] = in13;
		m_data[ 8 ] = in2;		m_data[ 9 ] = in6;		m_data[ 10 ] = in10;	m_data[ 11 ] = in14;
		m_data[ 12 ] = in3;		m_data[ 13 ] = in7;		m_data[ 14 ] = in11;	m_data[ 15 ] = in15;
	}
}
inline float& Matrix4x4::operator[]( int index )
{
	ASSERT_OR_DIE(index > -1 && index < 16, Stringf("Index %d into Matrix4x4::operator[] Out of Bounds", &index) );

	return m_data[ index ];
}
inline void mult( const Matrix4x4& whatToTransformIfRowMajor, const Matrix4x4& whatToTransformByIfRowMajor, Matrix4x4& out_result ) //Reverse args if col-major.
{
	const Matrix4x4& l = whatToTransformIfRowMajor;
	const Matrix4x4& r = whatToTransformByIfRowMajor;

	ASSERT_OR_DIE( &l != &out_result && &r != &out_result, "Cannot Mult-in-Place: Will Invalidate Entry Reuse in Later Multiplications!" );
	ASSERT_OR_DIE( l.GetOrdering() == r.GetOrdering(), "Cannot Mult Row-Major by Col-Major!" );

	//Credit to Guildhall's Math/Physics instructor, Anton Ephanov:

	//assume that the arguments are valid float[16] buffers
	out_result.m_data[ 0 ] = l.m_data[ 0 ] * r.m_data[ 0 ] + l.m_data[ 1 ] * r.m_data[ 4 ] + l.m_data[ 2 ] * r.m_data[ 8 ] + l.m_data[ 3 ] * r.m_data[ 12 ];
	out_result.m_data[ 1 ] = l.m_data[ 0 ] * r.m_data[ 1 ] + l.m_data[ 1 ] * r.m_data[ 5 ] + l.m_data[ 2 ] * r.m_data[ 9 ] + l.m_data[ 3 ] * r.m_data[ 13 ];
	out_result.m_data[ 2 ] = l.m_data[ 0 ] * r.m_data[ 2 ] + l.m_data[ 1 ] * r.m_data[ 6 ] + l.m_data[ 2 ] * r.m_data[ 10 ] + l.m_data[ 3 ] * r.m_data[ 14 ];
	out_result.m_data[ 3 ] = l.m_data[ 0 ] * r.m_data[ 3 ] + l.m_data[ 1 ] * r.m_data[ 7 ] + l.m_data[ 2 ] * r.m_data[ 11 ] + l.m_data[ 3 ] * r.m_data[ 15 ];

	out_result.m_data[ 4 ] = l.m_data[ 4 ] * r.m_data[ 0 ] + l.m_data[ 5 ] * r.m_data[ 4 ] + l.m_data[ 6 ] * r.m_data[ 8 ] + l.m_data[ 7 ] * r.m_data[ 12 ];
	out_result.m_data[ 5 ] = l.m_data[ 4 ] * r.m_data[ 1 ] + l.m_data[ 5 ] * r.m_data[ 5 ] + l.m_data[ 6 ] * r.m_data[ 9 ] + l.m_data[ 7 ] * r.m_data[ 13 ];
	out_result.m_data[ 6 ] = l.m_data[ 4 ] * r.m_data[ 2 ] + l.m_data[ 5 ] * r.m_data[ 6 ] + l.m_data[ 6 ] * r.m_data[ 10 ] + l.m_data[ 7 ] * r.m_data[ 14 ];
	out_result.m_data[ 7 ] = l.m_data[ 4 ] * r.m_data[ 3 ] + l.m_data[ 5 ] * r.m_data[ 7 ] + l.m_data[ 6 ] * r.m_data[ 11 ] + l.m_data[ 7 ] * r.m_data[ 15 ];

	out_result.m_data[ 8 ] = l.m_data[ 8 ] * r.m_data[ 0 ] + l.m_data[ 9 ] * r.m_data[ 4 ] + l.m_data[ 10 ] * r.m_data[ 8 ] + l.m_data[ 11 ] * r.m_data[ 12 ];
	out_result.m_data[ 9 ] = l.m_data[ 8 ] * r.m_data[ 1 ] + l.m_data[ 9 ] * r.m_data[ 5 ] + l.m_data[ 10 ] * r.m_data[ 9 ] + l.m_data[ 11 ] * r.m_data[ 13 ];
	out_result.m_data[ 10 ] = l.m_data[ 8 ] * r.m_data[ 2 ] + l.m_data[ 9 ] * r.m_data[ 6 ] + l.m_data[ 10 ] * r.m_data[ 10 ] + l.m_data[ 11 ] * r.m_data[ 14 ];
	out_result.m_data[ 11 ] = l.m_data[ 8 ] * r.m_data[ 3 ] + l.m_data[ 9 ] * r.m_data[ 7 ] + l.m_data[ 10 ] * r.m_data[ 11 ] + l.m_data[ 11 ] * r.m_data[ 15 ];

	out_result.m_data[ 12 ] = l.m_data[ 12 ] * r.m_data[ 0 ] + l.m_data[ 13 ] * r.m_data[ 4 ] + l.m_data[ 14 ] * r.m_data[ 8 ] + l.m_data[ 15 ] * r.m_data[ 12 ];
	out_result.m_data[ 13 ] = l.m_data[ 12 ] * r.m_data[ 1 ] + l.m_data[ 13 ] * r.m_data[ 5 ] + l.m_data[ 14 ] * r.m_data[ 9 ] + l.m_data[ 15 ] * r.m_data[ 13 ];
	out_result.m_data[ 14 ] = l.m_data[ 12 ] * r.m_data[ 2 ] + l.m_data[ 13 ] * r.m_data[ 6 ] + l.m_data[ 14 ] * r.m_data[ 10 ] + l.m_data[ 15 ] * r.m_data[ 14 ];
	out_result.m_data[ 15 ] = l.m_data[ 12 ] * r.m_data[ 3 ] + l.m_data[ 13 ] * r.m_data[ 7 ] + l.m_data[ 14 ] * r.m_data[ 11 ] + l.m_data[ 15 ] * r.m_data[ 15 ];
}


void Matrix4x4::GetAllValues( Matrix4x4& out_copy, Ordering* orderingReturned /*= nullptr*/ ) const
{
	const Ordering orderToUse = ( orderingReturned == nullptr ) ? m_ordering : *orderingReturned;

	if ( orderToUse == ROW_MAJOR )
	{
		if ( m_ordering == ROW_MAJOR )
			out_copy.SetAllValuesAssumingSameOrdering( this->m_data );
		else
			this->GetTranspose( out_copy );
	}
	if ( orderToUse == COLUMN_MAJOR )
	{
		if ( m_ordering == COLUMN_MAJOR )
			out_copy.SetAllValuesAssumingSameOrdering( this->m_data );
		else
			this->GetTranspose( out_copy );
	}
}
void Matrix4x4::GetTranspose( Matrix4x4& out_transpose ) const
{
	out_transpose.SetAllValuesAssumingSameOrdering( this->m_data );
	out_transpose.SetToTranspose();
}
void Matrix4x4::GetTranslation( Vector3& out_translation ) const
{
	if ( m_ordering == ROW_MAJOR ) //Implies translation == three bottom row cells, from the left.
	{
		out_translation.x = m_data[ 12 ];
		out_translation.y = m_data[ 13 ];
		out_translation.z = m_data[ 14 ];
	}
	else if ( m_ordering == COLUMN_MAJOR ) //Implies translation == three right column cells, from the top.
	{
		out_translation.x = m_data[ 3 ];
		out_translation.y = m_data[ 7 ];
		out_translation.z = m_data[ 11 ];
	}
}
void Matrix4x4::GetTranslation( Matrix4x4& out_translation, bool makeOtherCellsIdentity /*= false*/, Ordering* ordering /*= nullptr*/ ) const
{
	const Ordering orderToUse = ( ordering == nullptr ) ? m_ordering : *ordering;

	Vector3 currentTranslation;
	GetTranslation( currentTranslation );

	if ( makeOtherCellsIdentity )
		out_translation.ClearToTranslationMatrix( currentTranslation, orderToUse );
	else 
		out_translation.SetTranslation( currentTranslation, orderToUse );
}
void Matrix4x4::GetBasisDirectionI( Vector3& out_direction ) const
{
	if ( m_ordering == ROW_MAJOR ) //Implies basis.i == first row.
	{
		out_direction.x = m_data[ 0 ];
		out_direction.y = m_data[ 1 ];
		out_direction.z = m_data[ 2 ];
	}
	else if ( m_ordering == COLUMN_MAJOR ) //Implies basis.i == first column.
	{
		out_direction.x = m_data[ 0 ];
		out_direction.y = m_data[ 4 ];
		out_direction.z = m_data[ 8 ];
	}
}
void Matrix4x4::GetBasisDirectionJ( Vector3& out_direction ) const
{
	if ( m_ordering == ROW_MAJOR ) //Implies basis.j == second row.
	{
		out_direction.x = m_data[ 4 ];
		out_direction.y = m_data[ 5 ];
		out_direction.z = m_data[ 6 ];
	}
	else if ( m_ordering == COLUMN_MAJOR ) //Implies basis.j == second column.
	{
		out_direction.x = m_data[ 1 ];
		out_direction.y = m_data[ 5 ];
		out_direction.z = m_data[ 9 ];
	}
}
void Matrix4x4::GetBasisDirectionK( Vector3& out_direction ) const
{
	if ( m_ordering == ROW_MAJOR ) //Implies basis.k == third row.
	{
		out_direction.x = m_data[ 8 ];
		out_direction.y = m_data[ 9 ];
		out_direction.z = m_data[ 10 ];
	}
	else if ( m_ordering == COLUMN_MAJOR ) //Implies basis.k == third column.
	{
		out_direction.x = m_data[ 2 ];
		out_direction.y = m_data[ 6 ];
		out_direction.z = m_data[ 10 ];
	}
}
void Matrix4x4::GetRotation( Vector3& out_iDir, Vector3& out_jDir, Vector3& out_kDir ) const
{
	GetBasisDirectionI( out_iDir );
	GetBasisDirectionJ( out_jDir );
	GetBasisDirectionK( out_kDir );
}
void Matrix4x4::GetRotation( Matrix4x4& out_rotation, bool makeOtherCellsIdentity /*= false*/, Ordering* ordering /*= nullptr*/ ) const
{
	const Ordering orderToUse = ( ordering == nullptr ) ? m_ordering : *ordering;

	Vector3 iDir;
	Vector3 jDir;
	Vector3 kDir;
	GetRotation( iDir, jDir, kDir );

	if ( makeOtherCellsIdentity )
		out_rotation.ClearToRotationMatrix_ForsethBasis( iDir, jDir, kDir, orderToUse );
	else
		out_rotation.SetRotation( iDir, jDir, kDir, orderToUse );
}
void Matrix4x4::GetInverseAssumingOrthonormality( Matrix4x4& out_inverse ) const
{	
	Vector3 currentTranslationToInvertByNegation; //Inverse of translation == translation * -1.
	GetTranslation( currentTranslationToInvertByNegation );
	currentTranslationToInvertByNegation = -currentTranslationToInvertByNegation;

	Matrix4x4 invertedTranslationToRotate( m_ordering );
	invertedTranslationToRotate.SetTranslation( currentTranslationToInvertByNegation, m_ordering );

	Matrix4x4 rotationToInvertViaTranspose( m_ordering ); //Inverse of rotation == rotation^T.
	GetRotation( rotationToInvertViaTranspose );
	rotationToInvertViaTranspose.SetToTranspose();

	//(Rc*Tc)^-1 == (Tc^-1)*(Rc^-1) == (-Tc)*(Rc^T), i.e. transform the translation by the rotation.
	if ( m_ordering == ROW_MAJOR )
		mult( invertedTranslationToRotate, rotationToInvertViaTranspose, out_inverse );
	else if ( m_ordering == COLUMN_MAJOR )
		mult( rotationToInvertViaTranspose, invertedTranslationToRotate, out_inverse ); 
}


void Matrix4x4::Translate( const Vector3& translation )
{
	if ( m_ordering == ROW_MAJOR )
	{
		m_data[ 12 ] += translation.x;
		m_data[ 13 ] += translation.y;
		m_data[ 14 ] += translation.z;
	}
	else if ( m_ordering == COLUMN_MAJOR )
	{
		m_data[ 3 ] += translation.x;
		m_data[ 7 ] += translation.y;
		m_data[ 11 ] += translation.z;
	}
}
// void Matrix4x4::RotateAroundRightAxisI( float degrees )
// {
// 	//Compute new unsigned i, j, k vectors using canonical rotation matrix.
// 	float sinOfAngle = SinDegrees( degrees );
// 	float cosOfAngle = CosDegrees( degrees );
// 
// 	Vector3 iDir = Vector3( 1.f,		0.f,		0.f );
// 	Vector3 jDir = Vector3( 0.f, cosOfAngle, sinOfAngle );
// 	Vector3 kDir = Vector3( 0.f, sinOfAngle, cosOfAngle );
// 
// 	//Ordering determines where the canonical rotation matrix's negative goes.
// 	if ( m_ordering == ROW_MAJOR )
// 		kDir.y = -kDir.y;
// 	else if ( m_ordering == COLUMN_MAJOR )
// 		jDir.z = -jDir.z;
// 
// 	Rotate( iDir, jDir, kDir );
// }
// void Matrix4x4::RotateAroundUpAxisJ( float degrees )
// {
// 	//Compute new unsigned i, j, k vectors using canonical rotation matrix.
// 	float sinOfAngle = SinDegrees( degrees );
// 	float cosOfAngle = CosDegrees( degrees );
// 
// 	Vector3 iDir = Vector3( cosOfAngle,	0.f, sinOfAngle );
// 	Vector3 jDir = Vector3( 0.f,		1.f,		0.f );
// 	Vector3 kDir = Vector3( sinOfAngle, 0.f, cosOfAngle );
// 
// 	//Ordering determines where the canonical rotation matrix's negative goes.
// 	if ( m_ordering == ROW_MAJOR )
// 		iDir.z = -iDir.z;
// 	else if ( m_ordering == COLUMN_MAJOR )
// 		kDir.x = -kDir.x;
// 
// 	Rotate( iDir, jDir, kDir );
// }
// void Matrix4x4::RotateAroundForwardAxisK( float degrees )
// {
// 	//Compute new unsigned i, j, k vectors using canonical rotation matrix.
// 	float sinOfAngle = SinDegrees( degrees );
// 	float cosOfAngle = CosDegrees( degrees );
// 
// 	Vector3 iDir = Vector3( cosOfAngle, sinOfAngle, 0.f );
// 	Vector3 jDir = Vector3( sinOfAngle, cosOfAngle, 0.f );
// 	Vector3 kDir = Vector3( 0.f,		0.f,		1.f );
// 
// 	//Ordering determines where the canonical rotation matrix's negative goes.
// 	if ( m_ordering == ROW_MAJOR )
// 		jDir.x = -jDir.x;
// 	else if ( m_ordering == COLUMN_MAJOR )
// 		iDir.y = -iDir.y;
// 
// 	Rotate( iDir, jDir, kDir );
// }
void Matrix4x4::Rotate( const Vector3& iDir, const Vector3& jDir, const Vector3& kDir, Ordering ordering )
{
	//Create a matrix out of the arguments, transform *this matrix by it.
	Matrix4x4 rotationTransform( ordering );
	Matrix4x4 product( ordering );

	rotationTransform.SetRotation( iDir, jDir, kDir, ordering );

	mult( *this, rotationTransform, product );

	this->SetAllValuesAssumingSameOrdering( product );
}

void Matrix4x4::SetToTranspose( bool toggleOrdering /*= false*/ )
{
	//Inner 3x3.
	std::swap( m_data[ 1 ], m_data[ 4 ] );
	std::swap( m_data[ 2 ], m_data[ 8 ] );
	std::swap( m_data[ 6 ], m_data[ 9 ] );

	//Outermost.
	std::swap( m_data[ 12 ], m_data[ 3 ] );
	std::swap( m_data[ 13 ], m_data[ 7 ] );
	std::swap( m_data[ 14 ], m_data[ 11 ] );

	if ( toggleOrdering )
		m_ordering = ( m_ordering == ROW_MAJOR ) ? COLUMN_MAJOR : ROW_MAJOR;
}
void Matrix4x4::SetAllValuesAssumingSameOrdering( const Matrix4x4& inMatrix )
{
	inMatrix.GetAllValues( *this ); //Packs inMatrix's values INTO *this.
}
void Matrix4x4::SetAllValuesAssumingSameOrdering( const float* inArray )
{
	m_data[ 0 ] = inArray[ 0 ];		m_data[ 1 ] = inArray[ 1 ];		m_data[ 2 ] = inArray[ 2 ];		m_data[ 3 ] = inArray[ 3 ];
	m_data[ 4 ] = inArray[ 4 ];		m_data[ 5 ] = inArray[ 5 ];		m_data[ 6 ] = inArray[ 6 ];		m_data[ 7 ] = inArray[ 7 ];
	m_data[ 8 ] = inArray[ 8 ];		m_data[ 9 ] = inArray[ 9 ];		m_data[ 10 ] = inArray[ 10 ];	m_data[ 11 ] = inArray[ 11 ];
	m_data[ 12 ] = inArray[ 12 ];	m_data[ 13 ] = inArray[ 13 ];	m_data[ 14 ] = inArray[ 14 ];	m_data[ 15 ] = inArray[ 15 ];
}
void Matrix4x4::SetTranslation( const Vector3& translation, Ordering ordering )
{
	if ( ordering == ROW_MAJOR )
	{
		m_data[ 12 ] = translation.x;
		m_data[ 13 ] = translation.y;
		m_data[ 14 ] = translation.z;
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		m_data[ 3 ] = translation.x;
		m_data[ 7 ] = translation.y;
		m_data[ 11 ] = translation.z;
	}
}
void Matrix4x4::SetRotation( const Vector3& iDir, const Vector3& jDir, const Vector3& kDir, Ordering ordering )
{
	if ( ordering == ROW_MAJOR )
	{
		m_data[ 0 ] = iDir.x;	m_data[ 1 ] = iDir.y;	m_data[ 2 ] = iDir.z;
		m_data[ 4 ] = jDir.x;	m_data[ 5 ] = jDir.y;	m_data[ 6 ] = jDir.z;
		m_data[ 8 ] = kDir.x;	m_data[ 9 ] = kDir.y;	m_data[ 10 ] = kDir.z;
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		m_data[ 0 ] = iDir.x;	m_data[ 1 ] = jDir.x;	m_data[ 2 ] = kDir.x;
		m_data[ 4 ] = iDir.y;	m_data[ 5 ] = jDir.y;	m_data[ 6 ] = kDir.y;
		m_data[ 8 ] = iDir.z;	m_data[ 9 ] = jDir.z;	m_data[ 10 ] = kDir.z;
	}
}
void Matrix4x4::SetToLookFrom( const Vector3& translation, Ordering ordering )
{
	SetTranslation( translation, ordering );
}
void Matrix4x4::SetToLookAt( const Vector3& targetPos, const Vector3& globalUpDir, Ordering ordering )
{
	//Note the current matrix translation is being used here, since we have no parametric fromPos.
	Vector3 currentTranslation;
	this->GetTranslation( currentTranslation );
	SetToLookAt( currentTranslation, targetPos, globalUpDir, ordering );
}
void Matrix4x4::SetToLookAt( const Vector3& fromPos, const Vector3& targetPos, const Vector3& globalUpDir, Ordering ordering )
{
	//LookFrom.
	SetTranslation( fromPos, ordering );

	//LookAt.
	Vector3 localForward = targetPos - fromPos; //If inverted along the camera direction, try to reverse this difference?
	localForward.Normalize();

	Vector3 worldUp = globalUpDir;
	worldUp.Normalize();
	Vector3 localRight = CrossProduct( localForward, globalUpDir ); //Reverse the operand order if it needs to be left instead of right.
	localRight.Normalize();

	Vector3 localUp = CrossProduct( localRight, localForward );

	SetRotation( localRight, localUp, localForward, ordering );
}


void Matrix4x4::ClearToIdentityMatrix()
{
	m_data[ 0 ] = 1.0;	m_data[ 1 ] = 0.0;	m_data[ 2 ] = 0.0;	m_data[ 3 ] = 0.0;
	m_data[ 4 ] = 0.0;	m_data[ 5 ] = 1.0;	m_data[ 6 ] = 0.0;	m_data[ 7 ] = 0.0;
	m_data[ 8 ] = 0.0;	m_data[ 9 ] = 0.0;	m_data[ 10 ] = 1.0; m_data[ 11 ] = 0.0;
	m_data[ 12 ] = 0.0;	m_data[ 13 ] = 0.0; m_data[ 14 ] = 0.0; m_data[ 15 ] = 1.0;
}
void Matrix4x4::ClearToTranslationMatrix( const Vector3& translation, Ordering ordering )
{
	if ( ordering == ROW_MAJOR )
	{
		const float values[16] = {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			translation.x, translation.y, translation.z, 1.f
		};
		SetAllValuesAssumingSameOrdering( values );
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		const float values[16] = {
			1.f, 0.f, 0.f, translation.x,
			0.f, 1.f, 0.f, translation.y,
			0.f, 0.f, 1.f, translation.z,
			0.f, 0.f, 0.f, 1.f
		};
		SetAllValuesAssumingSameOrdering( values );
	}
}
void Matrix4x4::ClearToRotationMatrix_MyBasis( float yaw, float pitch, float roll, Ordering ordering )
{
	//Applies a C*R*C^-1 detoured version of the Forseth rotation transform.

	float sx = SinDegrees( pitch ); //assumes pitch == around i-vector == x
	float cx = CosDegrees( pitch );

	float sy = SinDegrees( yaw ); //assumes yaw == around j-vector == y
	float cy = CosDegrees( yaw );

	float sz = SinDegrees( roll ); //assumes roll == around k-vector == z
	float cz = CosDegrees( roll );

	if ( ordering == ROW_MAJOR )
	{

		const float values[ 16 ] =
		{
			cx*cz,					sx,						-cx*sz,						0.f, //i(-g)h
			-( -sy*sz - cy*cz*sx ),	cx*cy,					-cz*sy + cy*sx*sz,			0.f, //(-c)a(-b)
			-( cy*sz - cz*sx*sy ),	cx*sy,					cy*cz + sx*sy*sz,			0.f, //f(-d)e
			0.0f,					0.0f,					0.0f,						1.0f
		};
// 		const float values[ 16 ] = //Forseth's hand-computed x-then-y-then-z rotation transform composition.
// 		{
// 			cy*cz + sx*sy*sz,		-cx*sz,				-cz*sy + cy*sx*sz,			0.f,
// 			cy*sz - cz*sx*sy,		cx*cz,				-sy*sz - cy*cz*sx,			0.f,
// 			cx*sy,					sx,					cx*cy,						0.f,
// 			0.f,					0.f,				0.f,						1.0f
// 		};
		SetAllValuesAssumingSameOrdering( values );
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		const float values[ 16 ] =
		{
			cx*cy,					-(-cz*sy + cy*sx*sz),	-sy*sz - cy*cz*sx,			0.f, //i(-g)h
			-(cx*sy),				cy*cz + sx*sy*sz,		-(cy*sz - cz*sx*sy),		0.f, //(-c)a(-b)
			sx,						-(cx*sz),				cx*cz,						0.f, //f(-d)e
			0.0f,					0.0f,					0.0f,						1.0f
		};

// 		const float values[ 16 ] =
// 		{
// 			cy*cz + sx*sy*sz,	cy*sz - cz*sx*sy,		cx*sy,						0.f, //abc
// 			-cx*sz,				cx*cz,					sx,							0.f, //def
// 			-cz*sy + cy*sx*sz,	-sy*sz - cy*cz*sx,		cx*cy,						0.f, //ghi
// 			0.0f,				0.0f,					0.0f,						1.0f
// 		};
		SetAllValuesAssumingSameOrdering( values );
	}
}

void Matrix4x4::ClearToRotationMatrix_ForsethBasis( float yaw, float pitch, float roll, Ordering ordering )
{
	//EulerAngles angles = EulerAngles( roll, pitch, yaw ); //See this class for my engine conventions.

	//But below still uses the variable names from Forseth's left-handed y-up code. For him, x,y,z == pitch, yaw, roll
	float sx = SinDegrees( pitch ); //assumes pitch == around i-vector == x
	float cx = CosDegrees( pitch );

	float sy = SinDegrees( yaw ); //assumes yaw == around j-vector == y
	float cy = CosDegrees( yaw ); 

	float sz = SinDegrees( roll ); //assumes roll == around k-vector == z
	float cz = CosDegrees( roll );
	 
	if ( ordering == ROW_MAJOR )
	{
// 		const float values[ 16 ] = { //My hand-computed x-then-y-then-z rotation transform composition.
// 			cy*cz,					sx*sy*cz + cx*sz,		-cx*sy*cz + sx*sz,			0.f,
// 			-cy*sz,					-sx*sy*sz + cx*cz,		cx*sy*sz + sx*cz,			0.f,
// 			sy,						-sx*cy,					cx*cy,						0.f,
// 			0.f,					0.f,					0.f,						1.f
// 		};
		const float values[16] = //Forseth's hand-computed x-then-y-then-z rotation transform composition.
		{
			cy*cz + sx*sy*sz,		-cx*sz,				-cz*sy + cy*sx*sz,			0.f,
			cy*sz - cz*sx*sy,		cx*cz,				-sy*sz - cy*cz*sx,			0.f,
			cx*sy,					sx,					cx*cy,						0.f,
			0.f,					0.f,				0.f,						1.0f
		};
		SetAllValuesAssumingSameOrdering( values );
	}
	else if ( ordering == COLUMN_MAJOR )
	{
// 		const float values[ 16 ] = {
// 			cy*cz,				-cy*sz,					sy,			0.f,
// 			sx*sy*cz + cx*sz,	-sx*sy*sz + cx*cz,		-sx*cy,		0.f,
// 			-cx*sy*cz + sx*sz,	cx*sy*sz + sx*cz,		cx*cy,		0.f,
// 			0.f,				0.f,					0.f,		1.f
// 		};
		const float values[16] =
		{
			cy*cz + sx*sy*sz,	cy*sz - cz*sx*sy,		cx*sy,						0.f,
			-cx*sz,				cx*cz,					sx,							0.f,
			-cz*sy + cy*sx*sz,	-sy*sz - cy*cz*sx,		cx*cy,						0.f,
			0.0f,				0.0f,					0.0f,						1.0f
		};
		SetAllValuesAssumingSameOrdering( values );
	}
}
void Matrix4x4::ClearToRotationMatrix_ForsethBasis( const Vector3& iDir, const Vector3& jDir, const Vector3& kDir, Ordering ordering )
{
	if ( ordering == ROW_MAJOR )
	{
		const float values[16] = {
			iDir.x,		iDir.y,		iDir.z,		0.f,
			jDir.x,		jDir.y,		jDir.z,		0.f,
			kDir.x,		kDir.y,		kDir.z,		0.f,
			0.f,		0.f,		0.f,		1.f
		};
		SetAllValuesAssumingSameOrdering( values );
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		const float values[16] = {
			iDir.x,		jDir.x,		kDir.x,		0.f,
			iDir.y,		jDir.y,		kDir.y,		0.f,
			iDir.z,		jDir.z,		kDir.z,		0.f,
			0.f,		0.f,		0.f,		1.f
		};
		SetAllValuesAssumingSameOrdering( values );
	}
}
void Matrix4x4::ClearToLookFromMatrix( const Vector3& fromPos, Ordering ordering )
{
	ClearToTranslationMatrix( fromPos, ordering );
}
void Matrix4x4::ClearToLookAtMatrix( const Vector3& targetPos, const Vector3& globalUpDir, Ordering ordering )
{
	//Note the current matrix translation is being used here, since we have no parametric fromPos.
	Vector3 currentTranslation;
	this->GetTranslation( currentTranslation );
	ClearToLookAtMatrix( currentTranslation, targetPos, globalUpDir, ordering );
}
void Matrix4x4::ClearToLookAtMatrix( const Vector3& fromPos, const Vector3& targetPos, const Vector3& globalUpDir, Ordering ordering )
{
	ClearToIdentityMatrix();
	SetToLookAt( fromPos, targetPos, globalUpDir, ordering );
}
void Matrix4x4::ClearToPerspectiveProjection( const Matrix4x4& changeOfBasis, float fovDegreesY, float aspect, float zNear, float zFar, Ordering ordering ) //Projects things into not a box (as in ortho) but a frustum. Breaks if zNear <= 0. NOTE INVERTED Z!
{
	ASSERT_OR_DIE( zNear > 0, "Found Unallowed Non-positive zNear!" );

	//Create the view frustum bounds.
	Matrix4x4 perspectiveProjection( ordering );
	float rads = ConvertDegreesToRadians( fovDegreesY );
	float size = 1.f / tan( rads / 2.0f ); //Half of the horizon of the view frustum. Used to scale everything else by the fovDegreesY.

	float w = size;
	float h = size;
	if ( aspect > 1.0f )
		w /= aspect;
	else
		h *= aspect;

	float tmp = 1.0f / ( zFar - zNear );

	//Simplifies from general perspective projection when (as assumed here) right = -left, top = -bottom (i.e. symmetric frustum).
	if ( ordering == ROW_MAJOR )
	{
		const float values[16] = {
			w,			0.f,		0.f,					0.f,
			0.f,		h,			0.f,					0.f,
			0.f,		0.f,		-( zFar + zNear )*tmp,	-1.f, //z inverted by not having - sign precede this row's TWO entries!
			0.f,		0.f,		-2.0f*zNear*zFar*tmp,	0.f 
		};
		perspectiveProjection.SetAllValuesAssumingSameOrdering( values );
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		const float values[16] = {
			w,			0.f,		0.f,					0.f,
			0.f,		h,			0.f,					0.f,
			0.f,		0.f,		-( zFar + zNear )*tmp,	-2.0f*zNear*zFar*tmp,
			0.f,		0.f,		-1.f,					0.f //z inverted by not having - sign precede 3rd col's TWO entries!
		};
		perspectiveProjection.SetAllValuesAssumingSameOrdering( values );
	}

	//Combine change of basis and perspective matrices.
	if ( ordering == ROW_MAJOR )
		mult( changeOfBasis, perspectiveProjection, *this ); //Confirmed to be so if changeOfBasis is row-major, what about column
	else if ( ordering == COLUMN_MAJOR )
		mult( perspectiveProjection, changeOfBasis, *this );
}
void Matrix4x4::ClearToOrthogonalProjection( float width, float height, float zNear, float zFar, Ordering ordering ) //Puts a box at your camera, projects things into it.
{
	//Simplifies from general perspective projection when (as assumed here) right = -left, top = -bottom (i.e. symmetric frustum).

	float tmp = 1.0f / ( zFar - zNear );

	if ( ordering == ROW_MAJOR )
	{
		const float values[ 16 ] = {
			2.f / width,	0.f,			0.f,			0.f,
			0.f,			2.f / height,	0.f,			0.f,
			0.f,			0.f,			2.f*tmp,		-( zFar + zNear )*tmp,
			0.f,			0.f,			0.f,			1.f
		};
		SetAllValuesAssumingSameOrdering( values );
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		const float values[ 16 ] = {
			2.0f / width,	0.f,			0.f,					0.f,
			0.f,			2.f / height,	0.f,					0.f,
			0.f,			0.f,			2.f*tmp,				0.f,
			0.f,			0.f,			-( zFar + zNear )*tmp,	1.f
		};
		SetAllValuesAssumingSameOrdering( values );
	}
}
void Matrix4x4::ClearToOrthogonalProjection( float nx, float fx, float ny, float fy, float nz, float fz, Ordering ordering ) //Lets you map freely, e.g. mipmaps from 0 to 1 instead of above's -1 to 1.
{
	//Unsimplified version, won't assume right = -left, top = -bottom (i.e. symmetric frustum) like other version.

	float sx = 1.0f / ( fx - nx );
	float sy = 1.0f / ( fy - ny );
	float sz = 1.0f / ( fz - nz );

	if ( ordering == ROW_MAJOR )
	{
		const float values[ 16 ] = {
			2.0f * sx,  0.0f,       0.0f,       0.0f,
			0.0f,       2.0f * sy,  0.0f,       0.0f,
			0.0f,       0.0f,       2.0f * sz,  0.0f,
			-( fx + nx ) * sx, -( fy + ny ) * sy, -( fz + nz ) * sz,       1.0f,
		};
		SetAllValuesAssumingSameOrdering( values );
	}
	else if ( ordering == COLUMN_MAJOR )
	{
		const float values[ 16 ] = {
			2.0f * sx,  0.0f,       0.0f,       -( fx + nx ) * sx,
			0.0f,       2.0f * sy,  0.0f,       -( fy + ny ) * sy,
			0.0f,       0.0f,       2.0f * sz,  -( fz + nz ) * sz,
			0.0f,       0.0f,       0.0f,       1.0f,
		};
		SetAllValuesAssumingSameOrdering( values );
	}
}

void Matrix4x4::PrintDebugMatrix( Ordering* orderingReturned /*= nullptr*/ ) const
{
	const Ordering orderToUse = ( orderingReturned == nullptr ) ? m_ordering : *orderingReturned;

	if ( orderToUse == ROW_MAJOR )
	{
		DebuggerPrintf( "\nPrinting matrix using row-major ordering." );
		if ( m_ordering == ROW_MAJOR )
		{
			DebuggerPrintf( "\nMatrix was using row-major ordering.\n" );
			TODO( "Make these into a single call." );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 0 ], m_data[ 1 ], m_data[ 2 ], m_data[ 3 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 4 ], m_data[ 5 ], m_data[ 6 ], m_data[ 7 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 8 ], m_data[ 9 ], m_data[ 10 ], m_data[ 11 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 12 ], m_data[ 13 ], m_data[ 14 ], m_data[ 15 ] );
		}
		else
		{
			DebuggerPrintf( "\nMatrix was using column-major ordering.\n" );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 0 ], m_data[ 4 ], m_data[ 8 ], m_data[ 12 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 1 ], m_data[ 5 ], m_data[ 9 ], m_data[ 13 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 2 ], m_data[ 6 ], m_data[ 10 ], m_data[ 14 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 3 ], m_data[ 7 ], m_data[ 11 ], m_data[ 15 ] );
		}
	}
	else if ( orderToUse == COLUMN_MAJOR )
	{
		DebuggerPrintf( "Printing matrix using column-major ordering.\n" );
		if ( m_ordering == COLUMN_MAJOR )
		{
			DebuggerPrintf( "\nMatrix was using row-major ordering.\n" );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 0 ], m_data[ 1 ], m_data[ 2 ], m_data[ 3 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 4 ], m_data[ 5 ], m_data[ 6 ], m_data[ 7 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 8 ], m_data[ 9 ], m_data[ 10 ], m_data[ 11 ] );
			DebuggerPrintf( "%f %f %f %f", m_data[ 12 ], m_data[ 13 ], m_data[ 14 ], m_data[ 15 ] );
		}
		else
		{
			DebuggerPrintf( "\nMatrix was using column-major ordering.\n" );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 0 ], m_data[ 4 ], m_data[ 8 ], m_data[ 12 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 1 ], m_data[ 5 ], m_data[ 9 ], m_data[ 13 ] );
			DebuggerPrintf( "%f %f %f %f\n", m_data[ 2 ], m_data[ 6 ], m_data[ 10 ], m_data[ 14 ] );
			DebuggerPrintf( "%f %f %f %f", m_data[ 3 ], m_data[ 7 ], m_data[ 11 ], m_data[ 15 ] );
		}
	}
}