#include "stdafx.h"
#include "CObservable.h"

CObservable::CObservable()
{
}


CObservable::~CObservable()
{
}

void CObservable::registerCallback( const std::function<void()>& callback )
{
    m_vectorCallback.push_back( callback );
}

void CObservable::unregisterCallback( const std::function<void()>& arg )
{
    for (size_t index = 0; index < m_vectorCallback.size(); index++)
    {
        if (m_vectorCallback[index].target_type() == arg.target_type() && m_vectorCallback[index].target<void>() == arg.target<void>())
        {
            m_vectorCallback.erase( m_vectorCallback.begin() + index );

            return;
        }
    }
}

void CObservable::notify() const
{
    for (const auto& callback : m_vectorCallback)
    {
        callback();
    }
}
