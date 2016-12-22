#pragma once

class Vector3;

enum Ordering { ROW_MAJOR, COLUMN_MAJOR, NUM_ORDERINGS };
	//Row-major: the first 4 values are the first row of the matrix. Multiply by vertex*(Model*View*Projection transforms) unless you transpose by sending GL_TRUE when setting MVP's uniforms.
	//Col-major: the first 4 values are the first col of the matrix. Multiply by (Model*View*Projection transforms)*vertex unless you transpose by sending GL_TRUE when setting MVP's uniforms.

class Matrix4x4
{

private:
	//BE VERY CAREFUL THAT THE DATA IS THE FIRST ELEMENT OF THE CLASS, ELSE BELOW CODE IS SHIFTED BY OTHER MEMBERS OF MATRIX CLASS.
	float m_data[ 16 ];
	Ordering m_ordering;


public:
	Matrix4x4( Ordering ordering = ROW_MAJOR ) : m_ordering( ordering ) { SetAllValuesAssumingSameOrdering( Matrix4x4::IDENTITY ); }
	Matrix4x4( const float values[ 16 ], Ordering ordering = ROW_MAJOR ) : m_ordering( ordering ) { SetAllValuesAssumingSameOrdering( values ); }
	Matrix4x4( float in0, float in1, float in2, float in3, float in4, float in5, float in6, float in7, 
			   float in8, float in9, float in10, float in11, float in12, float in13, float in14, float in15, Ordering ordering = ROW_MAJOR );
	inline float& operator[]( int index );
	inline friend void mult( const Matrix4x4& whatToTransformIfRowMajor, const Matrix4x4& whatToTransformByIfRowMajor, Matrix4x4& res ); //Reverse args if col-major.

	static const Matrix4x4 IDENTITY;

	Ordering GetOrdering() const { return m_ordering; }
	void ToggleOrdering() { m_ordering = ( m_ordering == ROW_MAJOR ) ? COLUMN_MAJOR : ROW_MAJOR; }


	//Packed based on the current value of m_ordering.
	void GetTranspose( Matrix4x4& out_transpose ) const;
	void GetAllValues( Matrix4x4& out_copy, Ordering* orderingReturned = nullptr ) const; //Will use its m_ordering if one isn't supplied. e.g. If given row-major, while currently col-major, will return transpose.
	void GetTranslation( Vector3& out_translation ) const;
	void GetTranslation( Matrix4x4& out_translation, bool makeOtherCellsIdentity = false, Ordering* ordering = nullptr ) const; //Will use its m_ordering if one isn't supplied.
	void GetBasisDirectionI( Vector3& out_direction ) const;
	void GetBasisDirectionJ( Vector3& out_direction ) const;
	void GetBasisDirectionK( Vector3& out_direction ) const;
	void GetRotation( Vector3& out_iDir, Vector3 & out_jDir, Vector3 & out_kDir ) const;
	void GetRotation( Matrix4x4& out_rotation, bool makeOtherCellsIdentity = false, Ordering* ordering = nullptr ) const; //Will use its m_ordering if one isn't supplied.
	void GetInverseAssumingOrthonormality( Matrix4x4& out_inverse ) const;

	//Below methods WILL NOT affect any unparameterized cells.
	void Translate( const Vector3& translation );
//	void RotateAroundRightAxisI( float degrees );
//	void RotateAroundUpAxisJ( float degrees );
//	void RotateAroundForwardAxisK( float degrees );
	void Rotate( const Vector3& iDir, const Vector3& jDir, const Vector3& kDir, Ordering ordering ); //No clamping or wrapping safeguards in place yet!

	void SetToTranspose( bool toggleOrdering = false );
	void SetAllValuesAssumingSameOrdering( const float* inArray );
	void SetAllValuesAssumingSameOrdering( const Matrix4x4& inMatrix );
	void SetTranslation( const Vector3& translation, Ordering ordering ); //Only changes 3 values, use ClearToTranslationMatrix to blank rest.
	void SetRotation( const Vector3& iDir, const Vector3& jDir, const Vector3& kDir, Ordering ordering );
	void SetToLookFrom( const Vector3& translation, Ordering ordering );
	void SetToLookAt( const Vector3& targetPos, const Vector3& globalUpDir, Ordering ordering );
	void SetToLookAt( const Vector3& fromPos, const Vector3& targetPos, const Vector3& globalUpDir, Ordering ordering );

	//Below methods WILL blank all unparameterized cells to Identity matrix values.
	void ClearToIdentityMatrix();
	void ClearToLookFromMatrix( const Vector3& fromPos, Ordering ordering );
	void ClearToLookAtMatrix( const Vector3& targetPos, const Vector3& globalUpDir, Ordering ordering );
	void ClearToLookAtMatrix( const Vector3& fromPos, const Vector3& targetPos, const Vector3& globalUpDir, Ordering ordering );
	void ClearToRotationMatrix_MyBasis( float yaw, float pitch, float roll, Ordering ordering );
	void ClearToRotationMatrix_ForsethBasis( float yaw, float pitch, float roll, Ordering ordering );
	void ClearToRotationMatrix_ForsethBasis( const Vector3& iDir, const Vector3& jDir, const Vector3& kDir, Ordering ordering );
	void ClearToTranslationMatrix( const Vector3& translation, Ordering ordering );
	void ClearToPerspectiveProjection( const Matrix4x4& changeOfBasis, float fovDegreesY, float aspect, float zNear, float zFar, Ordering ordering );
	void ClearToOrthogonalProjection( float width, float height, float zNear, float zFar, Ordering ordering );
	void ClearToOrthogonalProjection( float nx, float fx, float ny, float fy, float nz, float fz, Ordering ordering );

	void PrintDebugMatrix( Ordering* orderingReturned = nullptr ) const; //Will use its m_ordering if one isn't supplied. e.g. If given row-major, while currently col-major, will return transpose.
};
